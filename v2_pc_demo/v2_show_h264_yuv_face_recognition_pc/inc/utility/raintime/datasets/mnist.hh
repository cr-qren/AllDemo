/*!\copyright
 * Copyright (c) 2018 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef RAINTIME_DATASET_MNIST_HH
#define RAINTIME_DATASET_MNIST_HH

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "raintime/core.h"

#include <glog/logging.h>

namespace raintime {
namespace dataset {

uint32_t read_header(const std::unique_ptr<char[]>& buffer, size_t pos);
std::unique_ptr<char[]> read_mnist_file(const std::string& path, uint32_t key);

template <typename T = uint8_t>
class MNISTImage : public Image {
 public:
  MNISTImage(T* buf, int image_size)
      : data_(buf, buf + image_size * image_size) {}

  std::vector<T> data() const { return data_; }

 private:
  std::vector<T> data_;
};

/**
 * https://github.com/wichtounet/mnist/
 */
template <typename ImageT = MNISTImage<uint8_t>, typename LabelT = int>
class MNISTDataset : public Dataset<ImageT, LabelT> {
 public:
  static constexpr int IMAGE_SIZE = 28;
  static constexpr int IMAGE_NUM_CHANNELS = 1;
  static constexpr int TRAIN_SIZE = 60000;
  static constexpr int TEST_SIZE = 10000;

  explicit MNISTDataset(std::string train_images_file,
                        std::string train_labels_file,
                        std::string test_images_file,
                        std::string test_labels_file);

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

  void ReadImagesFromFile(std::string images_file, int size,
                          std::vector<ImageT>& images);
  void ReadLabelsFromFile(std::string labels_file, int size,
                          std::vector<LabelT>& labels);
};

template <typename ImageT, typename LabelT>
MNISTDataset<ImageT, LabelT>::MNISTDataset(std::string train_images_file,
                                           std::string train_labels_file,
                                           std::string test_images_file,
                                           std::string test_labels_file) {
  auto train_images_buf = read_mnist_file(train_images_file, 0x803);
  auto train_labels_buf = read_mnist_file(train_labels_file, 0x801);
  auto test_images_buf = read_mnist_file(test_images_file, 0x803);
  auto test_labels_buf = read_mnist_file(test_labels_file, 0x801);

  auto num_train_images = read_header(train_images_buf, 1);
  auto train_image_size = read_header(train_images_buf, 2);
  CHECK_EQ(num_train_images, TRAIN_SIZE);
  CHECK_EQ(train_image_size, IMAGE_SIZE);

  ReadImagesFromFile(train_images_file, TRAIN_SIZE, train_images);
  ReadImagesFromFile(test_images_file, TEST_SIZE, test_images);
  ReadLabelsFromFile(train_labels_file, TRAIN_SIZE, train_labels);
  ReadLabelsFromFile(test_labels_file, TEST_SIZE, test_labels);

  CHECK_EQ(train_images.size(), TRAIN_SIZE);
  CHECK_EQ(test_images.size(), TEST_SIZE);
  CHECK_EQ(train_labels.size(), TRAIN_SIZE);
  CHECK_EQ(test_labels.size(), TEST_SIZE);
}

template <typename ImageT, typename LabelT>
void MNISTDataset<ImageT, LabelT>::ReadImagesFromFile(
    std::string images_file, int size, std::vector<ImageT>& images) {
  auto buf = read_mnist_file(images_file, 0x803);
  auto image_buf = reinterpret_cast<uint8_t*>(buf.get() + 16);

  images.reserve(size);
  for (int i = 0; i < size; i++) {
    images.push_back(ImageT(image_buf, IMAGE_SIZE));
    image_buf += IMAGE_SIZE * IMAGE_SIZE;
  }
}

template <typename ImageT, typename LabelT>
void MNISTDataset<ImageT, LabelT>::ReadLabelsFromFile(
    std::string labels_file, int size, std::vector<LabelT>& labels) {
  auto buf = read_mnist_file(labels_file, 0x801);
  auto label_buf = reinterpret_cast<uint8_t*>(buf.get() + 8);

  labels.reserve(size);
  for (int i = 0; i < size; i++) {
    labels.push_back(static_cast<LabelT>(*label_buf));
    label_buf++;
  }
}

uint32_t read_header(const std::unique_ptr<char[]>& buffer, size_t pos) {
  auto header = reinterpret_cast<uint32_t*>(buffer.get());

  auto value = *(header + pos);
  return (value << 24) | ((value << 8) & 0x00FF0000) |
         ((value >> 8) & 0X0000FF00) | (value >> 24);
}

std::unique_ptr<char[]> read_mnist_file(const std::string& path, uint32_t key) {
  std::ifstream file;
  std::cout << path << '\n';
  file.open(path, std::ios::in | std::ios::binary | std::ios::ate);

  if (!file) LOG(FATAL) << "Error opening file";

  auto size = file.tellg();
  std::unique_ptr<char[]> buffer(new char[size]);

  // Read the entire file at once
  file.seekg(0, std::ios::beg);
  file.read(buffer.get(), size);
  file.close();

  auto magic = read_header(buffer, 0);

  if (magic != key)
    LOG(FATAL) << "Invalid magic number, probably not a MNIST file";

  auto count = read_header(buffer, 1);

  if (magic == 0x803) {
    auto rows = read_header(buffer, 2);
    auto columns = read_header(buffer, 3);

    if (size < count * rows * columns + 16)
      LOG(FATAL) << "The file is not large enough to hold all the data, "
                    "probably corrupted";
  } else if (magic == 0x801) {
    if (size < count + 8)
      LOG(FATAL) << "The file is not large enough to hold all the data, "
                    "probably corrupted";
  }

  return buffer;
}
}  // namespace dataset
}  // namespace raintime

#endif
