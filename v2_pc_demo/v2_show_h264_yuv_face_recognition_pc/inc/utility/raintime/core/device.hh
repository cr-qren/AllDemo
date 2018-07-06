/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_DEVICE_HH
#define RAINTIME_DEVICE_HH

#include "plumber_ir/dfg_def.pb.h"

#include <glog/logging.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>

#include "raintime/core/allocator.hh"
#include "raintime/core/fpga_device_utils.hh"
#define ConvertAddr(mapped_addr, fpga_addr) \
  fpga_addr = mapped_addr - (size_t)mem_dev + DDR_MEM_BASE

using namespace plumber_ir;

namespace raintime {

class Device {};

class CpuDevice : public Device {};

class FpgaDevice : public Device {
 public:
  FpgaDevice() : base_addr_(DDR_MEM_BASE), num_bytes_(DDR_MEM_SIZE) {}

  /*! Use this function to access the singleton device object.
   * \return the singleton device object
   */
  static FpgaDevice *Global() {
    static auto global = new FpgaDevice;
    return global;
  }

  // TODO(vince): add assertions to check whether base_addr and num_bytes are
  // aligned
  size_t base_addr() { return base_addr_; }
  size_t num_bytes() { return num_bytes_; }

  void SetBaseAddr(size_t base_addr) {
    CHECK_EQ(base_addr % Allocator::kAllocAlign, 0);
    base_addr_ = base_addr;
  }

  bool IsInAddressRange(size_t addr) {
    return addr >= base_addr() && addr < (base_addr() + num_bytes());
  }

  /*! Open the register device
   */
  void OpenRegDevice() {
    // will not open the device unless the device is not opened
    if (reg_dev_fd != -1) return;

    raintime::OpenRegDevice(&reg_dev_fd, &reg_dev);

    if (reg_dev_fd < 0)
      throw std::runtime_error("Cannot correctly open the register device");
    if (reg_dev == MAP_FAILED) {
      close(reg_dev_fd);
      throw std::runtime_error(
          "Cannot allocate the memory space for the device");
    }
  }

  /*! Close the register device */
  void CloseRegDevice() {
    raintime::CloseRegDevice(reg_dev_fd, reg_dev);

    reg_dev_fd = -1;
    reg_dev = nullptr;
  }

  /*! Write register */
  void WriteReg(RegAddrT addr, RegDataT data) {
    raintime::WriteReg(addr, data, reg_dev);
  }

  /*! Read register */
  RegDataT ReadReg(RegAddrT addr) { return raintime::ReadReg(addr, reg_dev); }

  /*! Open the reset device */
  void OpenResetDevice() {
    if (reset_dev_fd != -1) return;
    raintime::OpenResetDevice(&reset_dev_fd, &reset_dev);

    if (reset_dev_fd < 0)
      throw std::runtime_error("Cannot correctly open the reset device");
    if (reset_dev == MAP_FAILED) {
      close(reset_dev_fd);
      throw std::runtime_error(
          "Cannot allocate the memory space for the reset device");
    }
  }

  /*! Close the reset device */
  void CloseResetDevice() {
    raintime::CloseResetDevice(reset_dev_fd, reset_dev);

    reset_dev_fd = -1;
    reset_dev = nullptr;
  }

  /*! Reset */
  void Reset() { raintime::ResetDevice(reset_dev); }

  /*! DMA Write */
  void WriteToFpga(RegAddrT addr, size_t size, uint32_t channel_id,
                   uint32_t dma_id) {
    raintime::WriteToFpga(reg_dev, addr, size, channel_id, dma_id);
  }

  /*! DMA Read */
  void ReadFromFpga(RegAddrT addr, size_t size, uint32_t channel_id,
                    uint32_t dma_id) {
    raintime::ReadFromFpga(reg_dev, addr, size, channel_id, dma_id);
  }

  void WriteToFpgaPV(int dma_id, RegAddrT addr, int sif_channel, int width,
                     int height, int stride) {
    size_t fpga_addr;
    ConvertAddr(addr, fpga_addr);
    raintime::WriteToFpgaPV(reg_dev, fpga_addr, sif_channel, width, height,
                            stride, dma_id);
  }

  void ReadFromFpgaPV(int dma_id, RegAddrT addr, int sif_channel, int width,
                      int height, int stride) {
    size_t fpga_addr;
    ConvertAddr(addr, fpga_addr);
    raintime::ReadFromFpgaPV(reg_dev, fpga_addr, sif_channel, width, height,
                             stride, dma_id);
  }

  void OpenMemDevice() {
    if (mem_dev_fd != -1) return;

    raintime::OpenMemDevice(&mem_dev_fd, &mem_dev);
    base_addr_ = (size_t)mem_dev;
    if (mem_dev_fd < 0)
      throw std::runtime_error("Cannot correctly open the mem device");
    if (mem_dev == MAP_FAILED) {
      close(mem_dev_fd);
      throw std::runtime_error(
          "Cannot allocate the memory space for the device");
    }
  }

  void CloseMemDevice() {
    raintime::CloseMemDevice(mem_dev_fd, mem_dev);
    base_addr_ = DDR_MEM_BASE;
    mem_dev_fd = -1;
    mem_dev = nullptr;
  }

  void LoadFile(char *file_name, void *addr, size_t size) {
    raintime::LoadFile(file_name, addr, size);
  }

  void DumpFile(char *file_name, void *addr, size_t size) {
    raintime::DumpFile(file_name, addr, size);
  }

  void *GetMemDev() { return mem_dev; }

 private:
  size_t base_addr_, num_bytes_;

  // register device
  int reg_dev_fd = -1;
  void *reg_dev = nullptr;

  // reset device
  int reset_dev_fd = -1;
  void *reset_dev = nullptr;

  // mem device;
  int mem_dev_fd = -1;
  void *mem_dev = nullptr;
};

#define DEVICE_CPU DFGNodeDef_Device_CPU
#define DEVICE_FPGA DFGNodeDef_Device_FPGA

// a type alias
using DeviceEnum = DFGNodeDef_Device;

inline const char *GetDeviceName(DeviceEnum device_enum) {
  switch (device_enum) {
    case DEVICE_CPU:
      return "CPU";
    case DEVICE_FPGA:
      return "FPGA";
    default:
      std::stringstream ss;
      ss << "Cannot recognise device enum: " << device_enum;

      throw std::runtime_error(ss.str());
  }
}

template <DFGNodeDef_Device device_enum>
struct DeviceTypeTraits {};

template <>
struct DeviceTypeTraits<DEVICE_CPU> {
  typedef CpuDevice DeviceType;
};

template <>
struct DeviceTypeTraits<DEVICE_FPGA> {
  typedef FpgaDevice DeviceType;
};
}  // namespace raintime

#endif  // RAINTIME_DEVICE_HH
