//=======================================================================
// Copyright (c) 2017 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

/*!
 * \file
 * \brief Contains functions to read the CIFAR-10 dataset
 */

#ifndef RAINTIME_DATASET_CIFAR_HH
#define RAINTIME_DATASET_CIFAR_HH

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "raintime/core.h"

namespace raintime {
namespace dataset {

template <typename T = uint8_t>
class CIFARImage : public Image {
 public:
  CIFARImage(T* buf, const int& n_channels, const int& image_size)
      : data_(buf, buf + n_channels * image_size * image_size) {}

  std::vector<T> data() const { return data_; }

 private:
  std::vector<T> data_;
};

void read_cifar10_file(std::vector<CIFARImage<uint8_t>>& images,
                       std::vector<int>& labels, const std::string& path,
                       const std::size_t& limit);

void read_test(const std::string& folder, const std::size_t& limit,
               std::vector<CIFARImage<uint8_t>>& images,
               std::vector<int>& labels);

void read_training(const std::string& folder, const std::size_t& limit,
                   std::vector<CIFARImage<uint8_t>>& images,
                   std::vector<int>& labels);

template <typename ImageT = CIFARImage<uint8_t>, typename LabelT = int>
class CIFARDataset : public Dataset<ImageT, LabelT> {
 public:
  static constexpr int IMAGE_SIZE = 32;
  static constexpr int IMAGE_NUM_CHANNELS = 3;
  static constexpr int TRAIN_SIZE = 50000;
  static constexpr int TEST_SIZE = 10000;

  explicit CIFARDataset(const std::string& path,
                        const int& training_limit = 50000,
                        const int& test_limit = 10000);

  int image_size() const { return IMAGE_SIZE; }
  int image_num_channels() const { return IMAGE_NUM_CHANNELS; }
  int train_size() const { return TRAIN_SIZE; }
  int test_size() const { return TEST_SIZE; }

  ImageT GetTrainImage(const int& idx) { return train_images[idx]; }
  LabelT GetTrainLabel(const int& idx) { return train_labels[idx]; }
  ImageT GetTestImage(const int& idx) { return test_images[idx]; }
  LabelT GetTestLabel(const int& idx) { return test_labels[idx]; }

 private:
  std::vector<ImageT> train_images, test_images;
  std::vector<LabelT> train_labels, test_labels;
};

template <typename ImageT, typename LabelT>
CIFARDataset<ImageT, LabelT>::CIFARDataset(const std::string& path,
                                           const int& training_limit,
                                           const int& test_limit) {
  read_training(path, training_limit, train_images, train_labels);
  read_test(path, test_limit, test_images, test_labels);
}

void read_cifar10_file(std::vector<CIFARImage<uint8_t>>& images,
                       std::vector<int>& labels, const std::string& path,
                       const std::size_t& limit) {
  if (limit && limit <= images.size()) {
    return;
  }

  std::ifstream file;
  file.open(path, std::ios::in | std::ios::binary | std::ios::ate);

  if (!file) {
    std::cout << "Error opening file: " << path << std::endl;
    return;
  }

  auto file_size = file.tellg();
  std::unique_ptr<char[]> buffer(new char[file_size]);

  // Read the entire file at once
  file.seekg(0, std::ios::beg);
  file.read(buffer.get(), file_size);
  file.close();

  std::size_t start = images.size();

  size_t size = 10000;
  size_t capacity = limit - images.size();

  if (capacity > 0 && capacity < size) {
    size = capacity;
  }

  // Prepare the size for the new
  images.reserve(images.size() + size);
  labels.resize(labels.size() + size);

  for (std::size_t i = 0; i < size; i++) {
    labels[start + i] = buffer[i * 3073];

    uint8_t* image_buffer = new uint8_t[32 * 32 * 3];

    for (std::size_t j = 1; j < 3073; j++) {
      image_buffer[j - 1] = buffer[i * 3073 + j];
    }
    images.push_back(CIFARImage<uint8_t>(image_buffer, 3, 32));
    delete[] image_buffer;
  }
}

void read_test(const std::string& folder, const std::size_t& limit,
               std::vector<CIFARImage<uint8_t>>& images,
               std::vector<int>& labels) {
  read_cifar10_file(images, labels, folder + "/test_batch.bin", limit);
}

void read_training(const std::string& folder, const std::size_t& limit,
                   std::vector<CIFARImage<uint8_t>>& images,
                   std::vector<int>& labels) {
  read_cifar10_file(images, labels, folder + "/data_batch_1.bin", limit);
  read_cifar10_file(images, labels, folder + "/data_batch_2.bin", limit);
  read_cifar10_file(images, labels, folder + "/data_batch_3.bin", limit);
  read_cifar10_file(images, labels, folder + "/data_batch_4.bin", limit);
  read_cifar10_file(images, labels, folder + "/data_batch_5.bin", limit);
}

}  // namespace dataset
}  // namespace raintime
#endif
