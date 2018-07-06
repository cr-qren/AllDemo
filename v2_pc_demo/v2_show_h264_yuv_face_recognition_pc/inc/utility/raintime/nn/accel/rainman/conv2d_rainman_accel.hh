/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_CONV2D_RAINMAN_ACCEL_HH
#define RAINTIME_CONV2D_RAINMAN_ACCEL_HH

#define CONV_REG_BASE (unsigned int)0x4000
#define CONV_REG_NUM_FRAMES (unsigned int)(CONV_REG_BASE)
#define CONV_REG_NUM_BLOCKS (unsigned int)(CONV_REG_BASE + 0x004)
#define CONV_REG_NUM_LAYERS (unsigned int)(CONV_REG_BASE + 0x008)
#define CONV_REG_NUM_FILTERS (unsigned int)(CONV_REG_BASE + 0x00C)
#define CONV_REG_NUM_CHANNELS (unsigned int)(CONV_REG_BASE + 0x010)
#define CONV_REG_NUM_DATA (unsigned int)(CONV_REG_BASE + 0x014)
#define CONV_REG_NUM_LINES (unsigned int)(CONV_REG_BASE + 0x018)
#define CONV_REG_NUM_ROWS (unsigned int)(CONV_REG_BASE + 0x01C)
#define CONV_REG_START_VALUE (unsigned int)(CONV_REG_BASE + 0x020)
#define CONV_REG_K_VALUE (unsigned int)(CONV_REG_BASE + 0x024)
#define CONV_REG_POOLING_EN (unsigned int)(CONV_REG_BASE + 0x028)
#define CONV_REG_CHANNEL_FINISH_VALUE (unsigned int)(CONV_REG_BASE + 0x040)
#define CONV_REG_COEFF_BASE (unsigned int)(CONV_REG_BASE + 0x100)
#define CONV_REG_CROP_LINES (unsigned int)(CONV_REG_BASE + 0x200)
#define CONV_REG_CROP_ROWS (unsigned int)(CONV_REG_BASE + 0x204)
#define CONV_REG_CROP_START_X (unsigned int)(CONV_REG_BASE + 0x208)
#define CONV_REG_CROP_START_Y (unsigned int)(CONV_REG_BASE + 0x20C)
#define CONV_REG_CROP_END_X (unsigned int)(CONV_REG_BASE + 0x210)
#define CONV_REG_CROP_END_Y (unsigned int)(CONV_REG_BASE + 0x214)
#define CONV_REG_CROP_EN (unsigned int)(CONV_REG_BASE + 0x218)
#define CONV_REG_PAD_LINE (unsigned int)(CONV_REG_BASE + 0x220)
#define CONV_REG_PAD_STATUS (unsigned int)(CONV_REG_BASE + 0x224)
#define CONV_REG_BN_EN (unsigned int)(CONV_REG_BASE + 0x228)
#define CONV_REG_BIAS_EN (unsigned int)(CONV_REG_BASE + 0x22C)
#define CONV_REG_RELU_EN (unsigned int)(CONV_REG_BASE + 0x230)
#define CONV_REG_COEFF_LEAK (unsigned int)(CONV_REG_BASE + 0x234)

#define CONV_DMA_ID (unsigned int)0x0
#define CONV_CROP_DMA_ID (unsigned int)0x1
#define CONV_DMA_COEFF_CHNL_ID (unsigned int)0x1
#define CONV_DMA_INPUT_CHNL_ID (unsigned int)0x0
#define CONV_DMA_OUTPT_CHNL_ID (unsigned int)0x0
#define CONV_DMA_CROP_CHNL_ID (unsigned int)0x1
#define CONV_DMA_BN_A_CHNL_ID (unsigned int)0x2
#define CONV_DMA_BN_B_CHNL_ID (unsigned int)0x3
#define CONV_DMA_BIAS_CHNL_ID (unsigned int)0x4

#define CONV_DEFAULT_NUM_FRAMES (unsigned int)1
#define CONV_DEFAULT_NUM_BLOCKS (unsigned int)1
#define CONV_DEFAULT_NUM_LAYERS (unsigned int)1
#define CONV_DEFAULT_POOL_STRIDE (unsigned int)2

#define CONV_MAX_BLOCK_SIZE (unsigned int)66
//#define DEBUG

#include <stdio.h>
#include "raintime/core/device.hh"

namespace raintime {
namespace nn {
namespace accel {
namespace rainman {

/*! Conv2D acceleration function.
 *
 * This function has the similar functionalities as `conv` in librainman,
 * however, there are several worth-to-note differences:
 * 1. Padding, BatchNorm, and Crop are temporarily removed
 * 2. MaxPooling enable boolean is specified with kernel size 2
 * 3. All enable booleans are prefixed with 'use_'
 *
 * \param dev The pointer to the FPGA device
 * \param height Height of the input feature map
 * \param width Width of the input feature map
 * \param num_channels Number of channels of the input feature map
 * \param num_filters Number of channels in the output feature map
 * \param kernel_size Size of the convolution kernel
 * \param use_max_pool_2x2 Whether to enable pooling or not.
 */
void Conv2DAccel(FpgaDevice *dev, int height, int width, int num_channels,
                 int num_filters, int kernel_size, int block_size,
                 bool use_padding, bool use_max_pool_2x2, bool use_bias,
                 bool use_relu, bool use_bn, size_t input_addr,
                 size_t coeff_addr, size_t output_addr, size_t bias_addr,
                 size_t a_addr, size_t b_addr);
}  // namespace rainman
}  // namespace accel
}  // namespace nn
}  // namespace raintime

#endif
