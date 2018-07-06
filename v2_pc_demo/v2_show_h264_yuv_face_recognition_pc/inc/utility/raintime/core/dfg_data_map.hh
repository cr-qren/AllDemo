/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef DFG_DATA_MAP_HH
#define DFG_DATA_MAP_HH

#include <glog/logging.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;

namespace raintime {
namespace dfg {

class DFGDataMap {
 public:
  using T = float;
  using KeyT = std::string;
  using PtrT = std::shared_ptr<std::vector<T>>;
  using ValT = std::pair<std::vector<int>, PtrT>;

  ~DFGDataMap() {
    // TODO(vince): fix the memory leak issue
  }

  json LoadConfig(const char* data_dir);

  std::vector<int> RestoreDimsFromShape(json::value_type shape) {
    std::vector<int> dims;
    for (auto dim : shape) dims.push_back(static_cast<int>(dim));

    return dims;
  }

  void LoadFromDir(const char* data_dir);
  void LoadInputFromDir(const char* data_dir,
                        std::vector<std::string>& input_nodes);

  template <typename T>
  void LoadImage(const std::vector<T>& image, const int& image_size,
                 const int& n_channels, const char* input_name,
                 const std::vector<int>& dims,
                 const std::string& normalizer = "none") {
    DFGDataMap::PtrT data = std::make_shared<std::vector<float>>(
        image_size * image_size * n_channels);
    if (normalizer == "mean") {
      for (int i = 0; i < image_size * image_size * n_channels; i++) {
        data->at(i) = ((float)image[i] - 128.0f) / 128.0f;
      }
    } else if (normalizer == "z") {
      for (int i = 0; i < n_channels; i++) {
        float mean = 0.0f;
        float mean_sqr = 0.0f;
        // Find the mean
        for (int j = 0; j < image_size * image_size; j++) {
          mean += (float)image[j + i * image_size * image_size];
          mean_sqr +=
              std::pow((float)image[j + i * image_size * image_size], 2);
        }
        mean /= image_size * image_size;
        mean_sqr /= image_size * image_size;
        float std = std::sqrt(mean_sqr - std::pow(mean, 2));
        for (int k = 0; k < image_size * image_size; k++) {
          data->at(k + i * image_size * image_size) =
              ((float)image[k + i * image_size * image_size] - mean) / std;
        }
      }
    } else if (normalizer == "vgg") {
      static const float arr[] = {123.68, 116.779, 103.939};
      std::vector<float> means(arr, arr + sizeof(arr) / sizeof(arr[0]));
      for (size_t i = 0; i < n_channels; i++) {
        for (int j = 0; j < image_size * image_size; j++) {
          data->at(j + i * image_size * image_size) =
              ((float)image[j + i * image_size * image_size] - means[i]);
        }
      }
    }

    else {
      for (int i = 0; i < image_size * image_size * n_channels; i++) {
        data->at(i) = (float)image[i];
      }
    }

    auto val = DFGDataMap::ValT(dims, data);
    set(input_name, val);
  }

  void StackImages(const std::vector<uint8_t>& images, const int& image_size,
                   const char* input_name, const std::vector<int>& dims,
                   const int& batch_size);

  void LoadGoldenFromDir(const char* data_dir,
                         std::vector<std::string>& golden_nodes);
  PtrT LoadDataFromFile(const char* data_dir, const char* file_key,
                        size_t num_elems);

  /**
   * \brief A helper function to insert a raw pointer into the data map.
   * \param key the data key
   * \param shape the shape of your data
   * \param raw_data the raw pointer
   */
  void InsertRawData(KeyT key, std::vector<int> shape, T* raw_data);

  ValT get(KeyT key) const {
    auto it = data_map_.find(key);
    if (it == data_map_.end())
      throw std::runtime_error("Cannot find key \"" + key +
                               "\" in the data map");

    return it->second;
  }

  void set(KeyT key, ValT& val) {
    CHECK(data_map_.find(key) == data_map_.end())
        << "Duplicated key found: " << key;

    keys_.push_back(key);
    data_map_.insert(std::pair<KeyT, ValT>(key, val));
  }

  void PrintKeys() const {
    for (auto it = data_map_.begin(); it != data_map_.end(); it++)
      std::cout << it->first << '\n';
  }

  // DEPRECATED
  bool hasKey(KeyT key) const { return data_map_.find(key) != data_map_.end(); }
  bool has_key(KeyT key) const {
    return data_map_.find(key) != data_map_.end();
  }

  std::vector<KeyT> keys() const { return keys_; }

 private:
  std::vector<KeyT> keys_;  // to keep keys' original sequence
  std::map<KeyT, ValT> data_map_;

  size_t GetNumElems(std::vector<int> dims) {
    size_t num_elems = 1;

    for (auto dim : dims) num_elems *= static_cast<size_t>(dim);

    return num_elems;
  }

  std::string GetConfigNamePrefix(std::string name) {
    auto pos = name.find('/');
    auto prev_pos = pos;
    while (pos != std::string::npos) {
      prev_pos = pos;
      pos = name.find('/', pos + 1);
    }

    return name.substr(0, prev_pos);
  }
};  // namespace dfg
}  // namespace dfg
}  // namespace raintime

#endif
