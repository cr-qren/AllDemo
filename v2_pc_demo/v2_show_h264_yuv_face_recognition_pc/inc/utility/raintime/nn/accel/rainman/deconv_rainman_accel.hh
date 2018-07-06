/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_DECONV_RAINMAN_ACCEL_HH
#define RAINTIME_DECONV_RAINMAN_ACCEL_HH

#define DECONV_REG_BASE (unsigned int)0x3000
#define DECONV_REG_CONTROL (unsigned int)(DECONV_REG_BASE)
#define DECONV_REG_STATUS (unsigned int)(DECONV_REG_BASE + 0x004)
#define DECONV_REG_NUM_ROWS (unsigned int)(DECONV_REG_BASE + 0x008)
#define DECONV_REG_NUM_COWS (unsigned int)(DECONV_REG_BASE + 0x00C)
#define DECONV_REG_NUM_CHANNELS (unsigned int)(DECONV_REG_BASE + 0x010)
#define DECONV_REG_K_VALUE (unsigned int)(DECONV_REG_BASE + 0x014)
#define DECONV_REG_P_VALUE (unsigned int)(DECONV_REG_BASE + 0x018)
#define DECONV_REG_S_VALUE (unsigned int)(DECONV_REG_BASE + 0x01C)
#define DECONV_REG_NUM_FILTERS (unsigned int)(DECONV_REG_BASE + 0x020)
#define DECONV_REG_NUM_TOTAL_DATA (unsigned int)(DECONV_REG_BASE + 0x024)
#define DECONV_REG_NUM_1C_DATA (unsigned int)(DECONV_REG_BASE + 0x028)
#define DECONV_REG_BIAS_EN (unsigned int)(DECONV_REG_BASE + 0x02C)
#define DECONV_REG_RELU_EN (unsigned int)(DECONV_REG_BASE + 0x030)

#define DECONV_DMA_ID (unsigned int)0x1
#define DECONV_DMA_COEFF_CHNL_ID (unsigned int)0x1
#define DECONV_DMA_INPUT_CHNL_ID (unsigned int)0x0
#define DECONV_DMA_OUTPT_CHNL_ID (unsigned int)0x0
#define DECONV_DMA_BIAS_CHNL_ID (unsigned int)0x2

#define DECONV_DEFAULT_STRIDE (unsigned int)2
//#define DEBUG

#include "raintime/core/device.hh"
#include <stdio.h>

namespace raintime {
namespace nn {
namespace accel {
namespace rainman {

/*! Deconv acceleration function.
 *
 * This function has the similar functionalities as `deconv` in librainman,
 * there are several worth-to-note differences:
 * 1. Padding, BatchNorm, and are temporarily removed
 * 2. All enable booleans are prefixed with 'use_'
 *
 * \param dev The pointer to the FPGA device
 * \param height Height of the input feature map
 * \param width Width of the input feature map
 * \param num_channels Number of channels of the input feature map
 * \param num_filters Number of channels in the output feature map
 * \param kernel_size Size of the convolution kernel
 */
void DeconvAccel(FpgaDevice *dev, int height, int width, int num_channels,
                 int num_filters, int kernel_size, bool use_bias, bool use_relu,
                 int input_addr, int coeff_addr, int output_addr,
                 int bias_addr) {
  int num_data;
  int output_size;
  int coeff_offset;
  unsigned int coeff_reg_addr;
  unsigned int output_offset = 0;

  if (!dev) throw std::runtime_error("Invalid FPGADevice!");
  if (height <= 0 || width <= 0 || num_channels <= 0)
    throw std::runtime_error("Shape of feature_maps should be positive!");
  if (num_filters <= 0 || kernel_size <= 0)
    throw std::runtime_error("Shape of conv_kernel should be positive!");
#ifdef DEBUG

  printf("[RAINTIME] Running DECONV ...\n");
  printf("[RAINTIME] height      : %08x\n", height);
  printf("[RAINTIME] width       : %08x\n", width);
  printf("[RAINTIME] channels    : %08x\n", num_channels);
  printf("[RAINTIME] filters     : %08x\n", num_filters);
  printf("[RAINTIME] kernel_size : %08x\n", kernel_size);
  printf("[RAINTIME] input_addr  : %08x\n", input_addr);
  printf("[RAINTIME] coeff_addr  : %08x\n", coeff_addr);
  printf("[RAINTIME] outpt_addr  : %08x\n", outpt_addr);
  printf("[RAINTIME] bias_en     : %08x\n", use_bias);
  printf("[RAINTIME] relu_en     : %08x\n", use_relu);
  printf("[RAINTIME] bias_addr   : %08x\n", bias_addr);

#endif

  // stride = kernel_size in U-net structure
  num_data = height * width * kernel_size * kernel_size;
  // Write registers
  dev->WriteReg(DECONV_REG_NUM_1C_DATA, width * height);
  dev->WriteReg(DECONV_REG_NUM_TOTAL_DATA, width * height * num_channels);
  dev->WriteReg(DECONV_REG_NUM_FILTERS, num_filters);
  dev->WriteReg(DECONV_REG_K_VALUE, kernel_size);
  dev->WriteReg(DECONV_REG_NUM_CHANNELS, num_channels);
  dev->WriteReg(DECONV_REG_NUM_COWS, width);
  dev->WriteReg(DECONV_REG_NUM_ROWS, height);
  dev->WriteReg(DECONV_REG_RELU_EN, use_relu);

  dev->WriteReg(DECONV_REG_CONTROL, 0);
  dev->WriteReg(DECONV_REG_CONTROL, 1);
  // write the input feature map from ARM to FPGA
  dev->WriteToFpga(input_addr,
                   height * width * num_channels * sizeof(unsigned int),
                   DECONV_DMA_INPUT_CHNL_ID, DECONV_DMA_ID);

  if (use_bias) {
    dev->WriteReg(DECONV_REG_BIAS_EN, use_bias);
    dev->WriteToFpga(bias_addr, num_filters * sizeof(unsigned int),
                     DECONV_DMA_BIAS_CHNL_ID, DECONV_DMA_ID);
  }
  // Run the computation.
  // for case kernel_size > 1, corresponding to 2*2 deconv in HW
  if (kernel_size > 1) {
    for (int f = 0; f < num_filters; f++) {
      coeff_offset = f * num_channels * kernel_size * kernel_size;
      dev->WriteToFpga(
          coeff_addr + coeff_offset,
          num_channels * kernel_size * kernel_size * sizeof(unsigned int),
          DECONV_DMA_COEFF_CHNL_ID, DECONV_DMA_ID);

      while ((dev->ReadReg(DECONV_REG_STATUS) & 0x01) != 0x01)
        ;

      /**
       * Read the output.
       * We assume that there is no padding.
       */
      output_size = num_data;
      // printf("[RAINTIME] output size: %d\n", output_size);
      dev->ReadFromFpga(output_addr + output_offset,
                        output_size * sizeof(unsigned int),
                        DECONV_DMA_OUTPT_CHNL_ID, DECONV_DMA_ID);
      output_offset += output_size;
    }
  //the rest corresponds to 1*1 deconv or conv in HW
  //We still fecth coeff param as in 2*2 
  } else if (num_filters < 5) {
    for (int c = 0; c < 1; c++) {
      coeff_offset = c * num_channels * kernel_size * kernel_size;
      dev->WriteToFpga(coeff_addr + coeff_offset,
                       num_channels * num_filters * kernel_size * kernel_size *
                           sizeof(unsigned int),
                       DECONV_DMA_COEFF_CHNL_ID, DECONV_DMA_ID);
      while ((dev->ReadReg(DECONV_REG_STATUS) & 0x01) != 0x01)
        ;

      output_size = num_filters * num_data;
      // printf("[RAINTIME] output size: %d\n", output_size);
      dev->ReadFromFpga(output_addr + output_offset,
                        output_size * sizeof(unsigned int),
                        DECONV_DMA_OUTPT_CHNL_ID, DECONV_DMA_ID);
      output_offset += output_size;
    }
  }

  else {
    for (int c = 0; c < num_filters / 4; c++) {
      coeff_offset = c * num_channels * 4 * kernel_size * kernel_size;
      dev->WriteToFpga(
          coeff_addr + coeff_offset,
          num_channels * 4 * kernel_size * kernel_size * sizeof(unsigned int),
          DECONV_DMA_COEFF_CHNL_ID, DECONV_DMA_ID);

      while ((dev->ReadReg(DECONV_REG_STATUS) & 0x01) != 0x01)
        ;

      output_size = 4 * num_data;
      // printf("[RAINTIME] output size: %d\n", output_size);
      dev->ReadFromFpga(output_addr + output_offset,
                        output_size * sizeof(unsigned int),
                        DECONV_DMA_OUTPT_CHNL_ID, DECONV_DMA_ID);
      output_offset += output_size;
    }

    if ((num_filters % 4) > 0)
      for (int c = 0; c < 1; c++) {
        coeff_offset =
            c * num_channels * num_filters % 4 * kernel_size * kernel_size;
        dev->WriteToFpga(coeff_addr + coeff_offset,
                         num_channels * num_filters % 4 * kernel_size *
                             kernel_size * sizeof(unsigned int),
                         DECONV_DMA_COEFF_CHNL_ID, DECONV_DMA_ID);
        while ((dev->ReadReg(DECONV_REG_STATUS) & 0x01) != 0x01)
          ;

        output_size = num_filters % 4 * num_data;
        // printf("[RAINTIME] output size: %d\n", outpt_size);
        dev->ReadFromFpga(output_addr + output_offset,
                          output_size * sizeof(unsigned int),
                          DECONV_DMA_OUTPT_CHNL_ID, DECONV_DMA_ID);
        output_offset += output_size;
      }
  }
}
}
}
}
}

#endif
