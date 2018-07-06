/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_NN_ACCEL_CPU_HH
#define RAINTIME_NN_ACCEL_CPU_HH

#include "raintime/core/fixed_point.hh"
#include "third_party/Eigen/Dense"

#include <glog/logging.h>
#include <cmath>

namespace raintime {
namespace nn {
namespace accel {
namespace cpu {

enum Conv2DAccelImpl { CONV2D_IM2COL, CONV2D_REFER };

#define DEF_CONV2D_ACCEL_IMPL Conv2DAccelImpl::CONV2D_IM2COL

/** Acceleration function for Conv2D on CPU.
 *
 * \param input_data shape C x H x W.
 * \param weights_data shape F x C x K x K.
 * \param bias_data shape F, could be nullptr
 * \param output_data shape F x OH x OW
 * \param height
 * \param width
 * \param num_channels
 * \param num_filters
 * \param kernel_size
 * \param pad
 * \param stride
 * \param use_relu whether to use Relu activation
 *
 * \tparam T scalar type.
 */
template <typename T, Conv2DAccelImpl impl = DEF_CONV2D_ACCEL_IMPL>
struct Conv2DCpuAccel {};

template <typename T>
struct ActivationCpuAccel;

template <typename T>
struct Conv2DCpuAccel<T, CONV2D_IM2COL> {
  void operator()(const T *input_data, const T *weights_data,
                  const T *bias_data, const T *bn_a_data, const T *bn_b_data,
                  T *output_data, int height, int width, int num_channels,
                  int num_filters, int kernel_size, int pad, int stride,
                  bool use_relu = false) {
    using MT = typename Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

    // typedef typename std::conditional<
    //     std::is_same<T, int32_t>::value,
    //     Eigen::Matrix<int64_t, Eigen::Dynamic, Eigen::Dynamic>,
    //     Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>::type MT;

    auto kernel_height = kernel_size;
    auto kernel_width = kernel_size;
    auto output_height = (height - kernel_size + 2 * pad) / stride + 1;
    auto output_width = (width - kernel_size + 2 * pad) / stride + 1;

    auto num_patches = output_height * output_width;
    auto num_cols = kernel_height * kernel_width * num_channels;

    // create matrices
    MT im2col_input(num_patches, num_cols);
    MT im2col_weights(num_cols, num_filters);
    MT im2col_output(num_patches, num_filters);

    int input_row_idx = 0, input_col_idx = 0;
    for (int c = 0; c < num_channels; c++) {
      for (int k_h = 0; k_h < kernel_height; k_h++) {
        for (int k_w = 0; k_w < kernel_width; k_w++) {
          // setup weights
          for (int f = 0; f < num_filters; f++) {
            int weights_data_idx =
                f * num_channels * kernel_height * kernel_width +
                c * kernel_height * kernel_width + k_h * kernel_width + k_w;
            im2col_weights(input_col_idx, f) = weights_data[weights_data_idx];
          }

          // setup input
          input_row_idx = 0;
          for (int out_h = 0; out_h < output_height; out_h++) {
            for (int out_w = 0; out_w < output_width; out_w++) {
              int in_h = out_h * stride + k_h - pad;
              int in_w = out_w * stride + k_w - pad;

              bool out_of_border =
                  in_h < 0 || in_h >= height || in_w < 0 || in_w >= width;

              im2col_input(input_row_idx, input_col_idx) =
                  out_of_border
                      ? static_cast<T>(0.0f)
                      : input_data[c * height * width + in_h * width + in_w];

              input_row_idx++;
            }
          }
          input_col_idx++;
        }
      }
    }

    // core GEMM
    im2col_output.noalias() = im2col_input * im2col_weights;

    // finalise output
    int output_row_idx = 0;
    for (int out_h = 0; out_h < output_height; out_h++) {
      for (int out_w = 0; out_w < output_width; out_w++) {
        for (int f = 0; f < num_filters; f++) {
          int output_data_idx =
              f * output_height * output_width + out_h * output_width + out_w;

          output_data[output_data_idx] =
              im2col_output(output_row_idx, f) +
              ((bias_data == nullptr) ? static_cast<T>(0.0f) : bias_data[f]);

          if (bn_a_data != nullptr && bn_b_data != nullptr) {
            output_data[output_data_idx] =
                output_data[output_data_idx] * bn_a_data[f] + bn_b_data[f];
          }

          if (use_relu)
            output_data[output_data_idx] =
                std::max(output_data[output_data_idx], static_cast<T>(0.0f));
        }
        output_row_idx++;
      }
    }
  }
};

template <typename T>
struct Conv2DCpuAccel<T, CONV2D_REFER> {
  void operator()(const T *input_data, const T *weights_data,
                  const T *bias_data, const T *bn_a_data, const T *bn_b_data,
                  T *output_data, int height, int width, int num_channels,
                  int num_filters, int kernel_size, int pad, int stride,
                  bool use_relu = false) {
    auto kernel_height = kernel_size;
    auto kernel_width = kernel_size;
    auto output_height = (height - kernel_size + 2 * pad) / stride + 1;
    auto output_width = (width - kernel_size + 2 * pad) / stride + 1;

    for (int f = 0; f < num_filters; f++) {
      for (int out_h = 0; out_h < output_height; out_h++) {
        for (int out_w = 0; out_w < output_width; out_w++) {
          int output_data_idx =
              f * output_height * output_width + out_h * output_width + out_w;

          output_data[output_data_idx] =
              (bias_data == nullptr) ? static_cast<T>(0.0f) : bias_data[f];

          for (int c = 0; c < num_channels; c++) {
            for (int kx = 0; kx < kernel_height; kx++) {
              for (int ky = 0; ky < kernel_width; ky++) {
                int in_h = out_h * stride + kx - pad;
                int in_w = out_w * stride + ky - pad;
                if (in_h < 0 || in_h >= height || in_w < 0 || in_w >= width)
                  continue;

                int input_idx = c * height * width + in_h * width + in_w;
                int weights_idx =
                    f * num_channels * kernel_height * kernel_width +
                    c * kernel_height * kernel_width + kx * kernel_width + ky;

                output_data[output_data_idx] +=
                    input_data[input_idx] * weights_data[weights_idx];
              }
            }
          }

          if (bn_a_data != nullptr && bn_b_data != nullptr) {
            output_data[output_data_idx] =
                output_data[output_data_idx] * bn_a_data[f] + bn_b_data[f];
          }
          if (use_relu)
            output_data[output_data_idx] =
                std::max(output_data[output_data_idx], static_cast<T>(0.0f));
        }
      }
    }
  }
};

/** Acceleration function for MaxPool2D on CPU.
 *
 * \param input_data shape C x H x W.
 * \param output_data shape C x H x W
 * \param height
 * \param width
 * \param num_channels
 * \param kernel_size
 * \param pad
 * \param stride
 *
 * \tparam T scalar type.
 */
template <typename T>
struct MaxPool2DCpuAccel {
  void operator()(const T *input_data, T *output_data, int height, int width,
                  int num_channels, int kernel_size, int pad, int stride) {
    auto kernel_height = kernel_size;
    auto kernel_width = kernel_size;
    auto output_height = (height - kernel_size + 2 * pad) / stride + 1;
    auto output_width = (width - kernel_size + 2 * pad) / stride + 1;

    for (int c = 0; c < num_channels; c++) {
      for (int out_h = 0; out_h < output_height; out_h++) {
        for (int out_w = 0; out_w < output_width; out_w++) {
          int output_data_idx =
              c * output_height * output_width + out_h * output_width + out_w;

          for (int kx = 0; kx < kernel_height; kx++) {
            for (int ky = 0; ky < kernel_width; ky++) {
              int in_h = out_h * stride + kx - pad;
              int in_w = out_w * stride + ky - pad;
              if (in_h < 0 || in_h >= height || in_w < 0 || in_w >= width)
                continue;

              int input_data_idx = c * height * width + in_h * width + in_w;

              output_data[output_data_idx] =
                  (kx == 0 && ky == 0) ? input_data[input_data_idx]
                                       : std::max(input_data[input_data_idx],
                                                  output_data[output_data_idx]);
            }
          }
        }
      }
    }
  }
};

template <typename T>
struct BatchNormCpuAccel {
  void operator()(const T *input_data, const T *bn_a_data, const T *bn_b_data,
                  T *output_data, int height, int width, int num_channels,
                  bool use_relu) {
    for (int c = 0; c < num_channels; c++) {
      for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
          int data_idx = c * height * width + h * width + w;
          output_data[data_idx] =
              bn_a_data[c] * input_data[data_idx] + bn_b_data[c];
          if (use_relu)
            output_data[data_idx] =
                std::max(output_data[data_idx], static_cast<T>(0.0f));
        }
      }
    }
  }
};

template <typename T>
struct SoftmaxCpuAccel {
  void operator()(const T *input_data, T *output_data, int num_classes,
                  int batch_size) {
    T sum, max;

    for (int i = 0; i < batch_size; i++) {
      sum = static_cast<T>(0.0f);

      // find max value
      for (size_t j = 0; j < num_classes; j++) {
        int idx = i * num_classes + j;
        if (j == 0 || max < input_data[idx]) {
          max = input_data[idx];
        }
      }

      for (int c = 0; c < num_classes; c++) {
        int idx = i * num_classes + c;
        output_data[idx] = std::exp(static_cast<float>(input_data[idx]) -
                                    (static_cast<float>(max)));
        sum += output_data[idx];
      }

      for (int c = 0; c < num_classes; c++) {
        int idx = i * num_classes + c;
        output_data[idx] = static_cast<T>(static_cast<float>(output_data[idx]) /
                                          static_cast<float>(sum));
      }
    }
  }
};

template <typename T>
struct LRNCpuAccel {
  void operator()(const T *input_data, T *output_data, const int &dim_size_1,
                  const int &dim_size_2, const int &dim_size_3,
                  const int &dim_size_4, const int &depth_radius,
                  const float &bias, const float &beta, const float &alpha) {
    for (int i = 0; i < dim_size_1; i++) {
      for (int j = 0; j < dim_size_2; j++) {
        for (int k = 0; k < dim_size_3; k++) {
          for (int z = 0; z < dim_size_4; z++) {
            T squared_sum = 0.0f;
            for (int d = z - depth_radius; d < z + depth_radius + 1; d++) {
              if ((d >= 0) && (d < dim_size_4)) {
                int idx = d + k * dim_size_4 + j * dim_size_4 * dim_size_3 +
                          i * dim_size_4 * dim_size_3 * dim_size_2;
                squared_sum += input_data[idx] * input_data[idx];
              }
            }
            int idx = z + k * dim_size_4 + j * dim_size_4 * dim_size_3 +
                      i * dim_size_4 * dim_size_3 * dim_size_2;
            output_data[idx] = static_cast<float>(
                static_cast<float>(input_data[idx]) /
                static_cast<float>(std::pow(
                    (bias + alpha * static_cast<float>(squared_sum)), beta)));
          }
        }
      }
    }
  }
};

// Fully connected layer
template <typename T>
struct FCCpuAccel {
  void operator()(const T *input_data, const T *weights_data,
                  const T *bias_data, T *output_data, const int &rows,
                  const int &columns, const int &batch_size) {
    using MT = typename Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
    MT input(columns, batch_size);
    MT weights(rows, columns);
    MT output(rows, batch_size);

    for (int i = 0; i < batch_size; i++) {
      for (int j = 0; j < columns; j++) {
        input(j, i) = input_data[i * columns + j];
      }
    }

    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        weights(i, j) = weights_data[i * columns + j];
      }
    }

    output.noalias() = weights * input;
    for (int i = 0; i < batch_size; i++) {
      for (int j = 0; j < rows; j++) {
        output_data[i * rows + j] = output(j, i) + bias_data[j];
      }
    }
  }  // namespace accel
};   // namespace nn

// Fully connected layer
template <typename T>
struct ArgmaxCpuAccel {
  void operator()(const T *input_data, T *output_data, const int &input_size,
                  const int &batch_size, const int &dimension = 1) {
    auto max_output = new T[batch_size];
    for (size_t i = 0; i < batch_size; i++) {
      for (size_t j = 0; j < input_size; j++) {
        if (j == 0 || max_output[i] < input_data[i * input_size + j]) {
          max_output[i] = input_data[i * input_size + j];
          output_data[i] = (float)j;
        }
      }
    }
    delete[] max_output;
  }

  void operator()(const T *input_data, T *output_data, const int &height,
                  const int &width, const int &batch_size,
                  const int &dimension) {
    if (dimension == 2) {
      auto max_output = new T[batch_size * height];
      for (size_t i = 0; i < batch_size; i++) {
        for (size_t k = 0; k < height; k++) {
          for (size_t j = 0; j < width; j++) {
            if ((j == 0) || (max_output[i * height + k] <
                             input_data[i * height * width + k * width + j])) {
              max_output[i * height + k] =
                  input_data[i * height * width + k * width + j];
              output_data[i * height + k] = (float)j;
            }
          }
        }
      }
      delete[] max_output;
    } else if (dimension == 1) {
      auto max_output = new T[batch_size * width];
      for (size_t i = 0; i < batch_size; i++) {
        for (size_t k = 0; k < width; k++) {
          for (size_t j = 0; j < height; j++) {
            if ((j == 0) || (max_output[i * width + k] <
                             input_data[i * height * width + k + j * width])) {
              max_output[i * width + k] =
                  input_data[i * height * width + k + j * width];
              output_data[i * width + k] = (float)j;
            }
          }
        }
      }
      delete[] max_output;
    }
  }
};  // namespace cpu

template <typename T>
struct ChannelToLastCpuAccel {
  void operator()(const T *input_data, T *output_data, const int &height,
                  const int &width, const int &num_channels) {
    for (int c = 0; c < num_channels; c++) {
      for (int i = 0; i < height * width; i++) {
        output_data[i * num_channels + c] = input_data[c * height * width + i];
      }
    }
  }
};
template <typename T>
struct ChannelToSecondCpuAccel {
  void operator()(const T *input_data, T *output_data, const int &height,
                  const int &width, const int &num_channels) {
    for (int c = 0; c < num_channels; c++) {
      for (int i = 0; i < height * width; i++) {
        output_data[c * height * width + i] = input_data[i * num_channels + c];
      }
    }
  }
};

template <typename T>
struct ActivationCpuAccel {
  void operator()(T *output_data, const std::string &activation,
                  const int &size) {
    if (activation == "Relu") {
      for (size_t i = 0; i < size; i++) {
        output_data[i] = std::max(output_data[i], static_cast<T>(0.0f));
      }
    } else if (activation == "Sigmoid") {
      for (size_t i = 0; i < size; i++) {
        output_data[i] =
            1 / (1.0f + std::exp(-1 * static_cast<float>(output_data[i])));
      }
    } else if (activation == "Linear") {
      for (size_t i = 0; i < size; i++) {
        output_data[i] = output_data[i];
      }
    } else if (activation == "Step") {
      for (size_t i = 0; i < size; i++) {
        output_data[i] = static_cast<T>(output_data[i] > static_cast<T>(0.0f));
      }
    } else if (activation == "Abs") {
      for (size_t i = 0; i < size; i++) {
        output_data[i] = std::abs(static_cast<float>(output_data[i]));
      }
    } else if (activation == "Tanh") {
      for (size_t i = 0; i < size; i++) {
        output_data[i] = std::tanh(static_cast<float>(output_data[i]));
      }
    } else {
      throw std::invalid_argument(
          "activation function could not be recognised.");
    }
  }
};

}  // namespace cpu
}  // namespace accel
}  // namespace nn
}  // namespace raintime

#endif
