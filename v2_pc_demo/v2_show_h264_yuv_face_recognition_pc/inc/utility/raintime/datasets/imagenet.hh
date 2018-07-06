#ifndef RAINTIME_DATASET_IMAGENET_HH
#define RAINTIME_DATASET_IMAGENET_HH

#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

#include "raintime/core.h"

namespace raintime {
namespace dataset {

int getdir(const std::string& dir, vector<std::string>& files) {
  DIR* dp;
  struct dirent* dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    std::cout << "Error(" << errno << ") opening " << dir << std::endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if ((std::string(dirp->d_name) != ".") &&
        (std::string(dirp->d_name) != "..")) {
      files.push_back(dirp->d_name);
    }
  }
  closedir(dp);
  return 0;
}

template <typename T = uint8_t>
class ImageNetImage : public Image {
 public:
  ImageNetImage(T* buf, const int& n_channels, const int& image_size)
      : data_(buf, buf + n_channels * image_size * image_size) {}

  std::vector<T> data() const { return data_; }

 private:
  std::vector<T> data_;
};

void read_imagenet_file(std::vector<ImageNetImage<uint8_t>>& images,
                        const std::string& path, const std::size_t& limit);

void read_imagenet_labels(std::vector<int>& labels, const std::string& path,
                          const std::size_t& limit);

void read_test(const std::string& folder, const std::size_t& limit,
               std::vector<ImageNetImage<uint8_t>>& images,
               std::vector<int>& labels);

void read_training(const std::string& folder, const std::size_t& limit,
                   std::vector<ImageNetImage<uint8_t>>& images,
                   std::vector<int>& labels);

template <typename ImageT = ImageNetImage<uint8_t>, typename LabelT = int>
class ImageNetDataset : public Dataset<ImageT, LabelT> {
 public:
  static constexpr int IMAGE_SIZE = 224;
  static constexpr int IMAGE_NUM_CHANNELS = 3;
  static constexpr int TRAIN_SIZE = 456567;
  static constexpr int TEST_SIZE = 40152;

  explicit ImageNetDataset(const std::string& path,
                           const int& training_limit = 456567,
                           const int& test_limit = 40152,
                           const bool& read_train = false);

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
ImageNetDataset<ImageT, LabelT>::ImageNetDataset(const std::string& path,
                                                 const int& training_limit,
                                                 const int& test_limit,
                                                 const bool& read_train) {
  if (read_train)
    read_training(path, training_limit, train_images, train_labels);
  read_test(path, test_limit, test_images, test_labels);
}

void read_imagenet_file(std::vector<ImageNetImage<uint8_t>>& images,
                        const std::string& path, const std::size_t& limit) {
  int img_size = 224;
  int n_channels = 3;
  if (limit && limit <= images.size()) {
    return;
  }

  cv::Mat image;
  image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
  std::size_t start = images.size();

  size_t capacity = limit - images.size();

  uint8_t* image_buffer = new uint8_t[img_size * img_size * n_channels];

  int offset_y = (image.rows - img_size) / 2;
  int offset_x = (image.cols - img_size) / 2;

  for (int i = 0; i < n_channels; i++) {
    for (int j = 0; j < img_size; j++) {
      for (int k = 0; k < img_size; k++) {
        if ((offset_y >= 0) && (offset_x >= 0)) {
          cv::Vec3b colour =
              image.at<cv::Vec3b>(cv::Point(k + offset_x, j + offset_y));
          image_buffer[i * img_size * img_size + j * img_size + k] =
              colour.val[i];
        } else if ((offset_y < 0) && (offset_x < 0)) {
          if ((k < -1 * offset_x) || (j < -1 * offset_y) ||
              (k + offset_x >= image.cols) || (j + offset_y >= image.rows)) {
            image_buffer[i * img_size * img_size + j * img_size + k] = 255;
          } else {
            cv::Vec3b colour =
                image.at<cv::Vec3b>(cv::Point(k + offset_x, j + offset_y));
            image_buffer[i * img_size * img_size + j * img_size + k] =
                colour.val[i];
          }
        } else if ((offset_y < 0) && (offset_x > 0)) {
          if ((j < -1 * offset_y) || (j + offset_y >= image.rows)) {
            image_buffer[i * img_size * img_size + j * img_size + k] = 255;
          } else {
            cv::Vec3b colour =
                image.at<cv::Vec3b>(cv::Point(k + offset_x, j + offset_y));
            image_buffer[i * img_size * img_size + j * img_size + k] =
                colour.val[i];
          }
        } else if ((offset_x < 0) && (offset_y > 0)) {
          if ((k < -1 * offset_x) || (k + offset_x >= image.cols)) {
            image_buffer[i * img_size * img_size + j * img_size + k] = 255;
          } else {
            cv::Vec3b colour =
                image.at<cv::Vec3b>(cv::Point(k + offset_x, j + offset_y));
            image_buffer[i * img_size * img_size + j * img_size + k] =
                colour.val[i];
          }
        }
      }
    }
  }

  images.push_back(ImageNetImage<uint8_t>(image_buffer, n_channels, img_size));

  delete[] image_buffer;
}

void read_imagenet_labels(std::vector<int>& labels, const std::string& path,
                          const std::size_t& limit) {
  std::ifstream infile(path);
  int counter = 0;
  int label;
  while ((infile >> label) && (counter < limit)) {
    labels.push_back(label);
    counter++;
  }
  infile.close();
}

void read_test(const std::string& folder, const std::size_t& limit,
               std::vector<ImageNetImage<uint8_t>>& images,
               std::vector<int>& labels) {
  std::vector<std::string> files;
  getdir(folder + "/images/", files);
  for (size_t i = 0; i < files.size(); i++) {
    if (limit > images.size()) {
      read_imagenet_file(images, folder + "/images/" + files[i], limit);
    }
  }
  read_imagenet_labels(labels, folder + "/labels/" + "test_labels.txt", limit);
}

void read_training(const std::string& folder, const std::size_t& limit,
                   std::vector<ImageNetImage<uint8_t>>& images,
                   std::vector<int>& labels) {
  std::vector<std::string> files;
  getdir(folder + "/images/", files);
  for (auto p : files) {
    if (limit > images.size()) {
      read_imagenet_file(images, folder + "/images/" + p, limit);
    }
  }
  read_imagenet_labels(labels, folder + "/labels/" + "train_labels.txt", limit);
}

}  // namespace dataset
}  // namespace raintime
#endif
