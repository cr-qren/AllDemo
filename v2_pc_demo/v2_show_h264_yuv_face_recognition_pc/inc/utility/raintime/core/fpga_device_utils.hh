/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_FPGA_DEVICE_UTILS_HH
#define RAINTIME_FPGA_DEVICE_UTILS_HH

// These are base address for each APB slave
#define ADDR_DMA0_BASE 0x0000
#define ADDR_DMA2_BASE 0x1000
// Command
#define CTRL_CMD_M2L 0x02 // write from memory to logic
#define CTRL_CMD_L2M 0x03 // read from logic to memory
// DDR Mapping
#define DDR_MEM_BASE 0x01000000
#define DDR_MEM_SIZE 0x20000000
#define DDR_MEM_TMP_BASE 0x12100000
//#define DDR_MEM_TMP_BASE   0x3F000000    //changed for unet
#define DDR_MEM_DEVICE "/dev/mem"
#define DDR_REG_BASE 0x41230000
#define RESET_REG_BASE 0xF8000000

#define RESET_REG_ADDR 0x240
#define MMAP_SIZE 0xFFFF
#define RESET_SIZE 0xFFF

#define ADDR_STATUS 0x000
#define ADDR_CONTROL 0x004
#define ADDR_ID 0x008
#define ADDR_MADDR 0x00C
#define ADDR_CHNL 0x010
#define ADDR_BTT 0x014
//DMA FOR PV=16
#define ENGINE_BASE 0x0000
#define DMA0_BASE   0x1000
#define DMA2_BASE   0x2000

#define REG_DMA_ID      0x000
#define REG_DMA_STATUS  0x004
#define REG_DMA_RCTRL   0x010
#define REG_DMA_RMADDR  0x014
#define REG_DMA_RSIZE   0x018
#define REG_DMA_RCFG    0x01C
#define REG_DMA_WCTRL   0x020
#define REG_DMA_WMADDR  0x024
#define REG_DMA_WSIZE   0x028
#define REG_DMA_WCFG    0x02C

#define CONTROL_IC 0x8  // Clear IRQ

#define DMA_ADDR_MASK(dma_id) ((unsigned int)((dma_id) << 12))
#define READ_STATUS(x, dma_id, mapped_reg_ptr) \
  x = ReadReg(DMA_ADDR_MASK(dma_id) + ADDR_STATUS, mapped_reg_ptr)
#define WRITE_ID(x, dma_id, mapped_reg_ptr) \
  WriteReg(DMA_ADDR_MASK(dma_id) + ADDR_ID, x, mapped_reg_ptr)
#define WRITE_MADDR(x, dma_id, mapped_reg_ptr) \
  WriteReg(DMA_ADDR_MASK(dma_id) + ADDR_MADDR, x, mapped_reg_ptr)
#define WRITE_CHNL(x, dma_id, mapped_reg_ptr) \
  WriteReg(DMA_ADDR_MASK(dma_id) + ADDR_CHNL, x, mapped_reg_ptr)
#define WRITE_BTT(x, dma_id, mapped_reg_ptr) \
  WriteReg(DMA_ADDR_MASK(dma_id) + ADDR_BTT, x, mapped_reg_ptr)
#define WRITE_CONTROL(x, dma_id, mapped_reg_ptr) \
  WriteReg(DMA_ADDR_MASK(dma_id) + ADDR_CONTROL, x, mapped_reg_ptr)
#define CLEAR_CONTROL_IRQ(dma_id, mapped_reg_ptr) \
  WRITE_CONTROL((CONTROL_IC), dma_id, mapped_reg_ptr)

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>

namespace raintime {

typedef uint32_t RegAddrT;
typedef uint32_t RegDataT;

/*! Open the register device
 * \param reg_dev_fd The pointer to where the device file descriptor is.
 * \param reg_dev The pointer to the pointer to the memory space that will be
 * mapped.
 */
void OpenRegDevice(int *reg_dev_fd, void **reg_dev);

/*! Close the register device
 * \param reg_dev_fd The file descriptor for the register device.
 * \param reg_dev The pointer to the memory space that has been mapped.
 */
void CloseRegDevice(int reg_dev_fd, void *reg_dev);

/*! Open and reset the register device
 * \param reg_dev_fd The pointer to where the device file descriptor is.
 * \param reg_dev The pointer to the pointer to the memory space that will be
 * mapped.
 */
void OpenResetDevice(int *reset_dev_fd, void **reset_dev);

/*! Close and reset the register device
 * \param reg_dev_fd The file descriptor for the register device.
 * \param reg_dev The pointer to the memory space that has been mapped.
 */
void CloseResetDevice(int reset_dev_fd, void *reset_dev);

/*! Write data into register
 * \param addr The address of register
 * \param data The data should be write into the register
 * \param reg_dev The pointer to the memory space that has been mapped.
 */
void WriteReg(RegAddrT addr, RegDataT data, void *reg_dev);

/*! Read data from register
 * \param addr The address of register
 * \param reg_dev The pointer to the memory space that has been mapped.
 */
RegDataT ReadReg(RegAddrT addr, void *reg_dev);

void ResetDevice(void *reset_dev);

/**
 * Write data from ARM memory to DMA on FPGA.
 *
 * This write operation is processed through the IOCTL system call.
 * Note that the size of the transfer will be one byte less than
 * the actuall transfer size. However, you don't need to take care
 * of it - this function will handle this inside.
 *
 * \para reg_dev The pointer to the memory space that has been mapped.
 * \param addr The address in the ARM memory to be written
 * \param size The size of bytes to be written
 * \param channel_id The ID of the DMA memory-to-logic channel
 * \param dma_id The ID of the DMA, starting from 0.
 */
void WriteToFpga(void *reg_dev, RegAddrT addr, size_t size, uint32_t channel_id,
                 uint32_t dma_id);

/**
 * Read data from FPGA to ARM memory.
 *
 * \para reg_dev The pointer to the memory space that has been mapped.
 * \param addr The address in the ARM memory to be written
 * \param size The size of bytes to be written
 * \param channel_id The ID of the DMA logic-to-memory channel
 * \param dma_id The ID of the DMA, starting from 0.
 */
void ReadFromFpga(void *reg_dev, RegAddrT addr, size_t size,
                  uint32_t channel_id, uint32_t dma_id);

void WriteToFpgaPV(void *reg_dev, RegAddrT addr, uint32_t sif_channel, int width, int height,
                 int stride, uint32_t dma_id);

void ReadFromFpgaPV(void *reg_dev, RegAddrT addr, uint32_t sif_channel, int width, int height,
                 int stride, uint32_t dma_id);

/*! Open the mem device
 * \param mem_dev_fd The pointer to where the device file descriptor is.
 * \param mem_dev The pointer to the pointer to the memory space that will be
 * mapped.
 */
void OpenMemDevice(int *mem_dev_fd, void **mem_dev);

/*! Close the memory device
 * \param mem_dev_fd The file descriptor for the mem device.
 * \param mem_dev The pointer to the memory space that has been mapped.
 */
void CloseMemDevice(int mem_dev_fd, void *mem_dev);

void LoadFile(char *file_name, void *addr, size_t size);

void DumpFile( char *file_name, void *addr, size_t size);

}

#endif
