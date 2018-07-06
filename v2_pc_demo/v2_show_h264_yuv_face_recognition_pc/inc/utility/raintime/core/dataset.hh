/*!&\copyright
 * Copyright (c) 2018 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */
#ifndef RAINTIME_DATASET_HH
#define RAINTIME_DATASET_HH

#include <memory>

namespace raintime {
namespace dataset {

class Image {};

/**
 * Base class for defining a dataset.
 */
template <typename ImageT = Image, typename LabelT = int>
class Dataset {
 public:
  virtual int image_size() const = 0;
  virtual int image_num_channels() const = 0;
  virtual int train_size() const = 0;
  virtual int test_size() const = 0;

  virtual ImageT GetTrainImage(const int& idx) = 0;
  virtual LabelT GetTrainLabel(const int& idx) = 0;
  virtual ImageT GetTestImage(const int& idx) = 0;
  virtual LabelT GetTestLabel(const int& idx) = 0;
};
}  // namespace dataset
}  // namespace raintime

#endif
