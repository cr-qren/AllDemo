/**\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef LIBSSD_BBOX_HH
#define LIBSSD_BBOX_HH

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace libssd {

const int NUM_REFS = 4;

/*! The class representing the bounding box. */
class BBox {
 public:
  BBox(int class_idx, float score, float xmin, float ymin, float xmax,
       float ymax)
      : class_idx_(class_idx),
        score_(score),
        xmin_orig_(xmin),
        ymin_orig_(ymin),
        xmax_orig_(xmax),
        ymax_orig_(ymax) {
    // validate input parameter values
    if (class_idx < 0)
      throw std::invalid_argument("class_idx should be larger or equal to 0");
    if (score < 0.0f || score > 1.0f) {
      printf("the outrange score is %f\n", score);
      throw std::invalid_argument("score should be within range [0.0, 1.0]");
    }
    if (xmin > xmax)
      throw std::invalid_argument("xmin should be less than or equal to xmax");
    if (ymin > ymax)
      throw std::invalid_argument("ymin should be less than or equal to ymax");

    xmin_ = std::max(0.0f, xmin_orig_);
    ymin_ = std::max(0.0f, ymin_orig_);
    xmax_ = std::min(1.0f, xmax_orig_);
    ymax_ = std::min(1.0f, ymax_orig_);

    // cache volume value
    this->volume_ = (xmax_ - xmin_) * (ymax_ - ymin_);
  }

  BBox &operator=(const BBox &rhs) {
    class_idx_ = rhs.class_idx();
    score_ = rhs.score();
    xmin_ = rhs.xmin();
    xmax_ = rhs.xmax();
    ymin_ = rhs.ymin();
    ymax_ = rhs.ymax();
    volume_ = (xmax_ - xmin_) * (ymax_ - ymin_);
    return *this;
  }

  /*! Get the volume of the bounding box.
   * \return volume value
   */
  float volume() const { return this->volume_; }

  /*! Get the score for classification in the bounding box.
   * \return score value
   */
  float score() const { return this->score_; }
  int class_idx() const { return this->class_idx_; }

  float xmin() const { return this->xmin_; }
  float ymin() const { return this->ymin_; }
  float xmax() const { return this->xmax_; }
  float ymax() const { return this->ymax_; }

  float xmin_orig() const { return this->xmin_orig_; }
  float ymin_orig() const { return this->ymin_orig_; }
  float xmax_orig() const { return this->xmax_orig_; }
  float ymax_orig() const { return this->ymax_orig_; }

  /*! The overloaded comparison operator for sorting.
   * \return boolean indicating less or not */
  bool operator<(const BBox &rhs) const { return score_ < rhs.score(); }

  /*! Compute the Jaccard index between this and another bounding boxes
   * \param bbox another bounding box
   * \return the index value
   */
  float ComputeJaccardIndex(const BBox &bbox) const {
    float int_xmin = std::max(xmin_, bbox.xmin());
    float int_xmax = std::min(xmax_, bbox.xmax());
    float int_ymin = std::max(ymin_, bbox.ymin());
    float int_ymax = std::min(ymax_, bbox.ymax());

    float int_h = std::max(int_ymax - int_ymin, 0.0f);
    float int_w = std::max(int_xmax - int_xmin, 0.0f);
    float int_volume = int_h * int_w;

    return int_volume / (volume_ + bbox.volume() - int_volume);
  }

  // private:
  int class_idx_;
  float score_;
  float xmin_, ymin_, xmax_, ymax_;
  float xmin_orig_, ymin_orig_, xmax_orig_, ymax_orig_;
  float volume_;
};

/*! Perform the NMS filtering algorithm.
 * \param bboxes a list of bboxes to be filtered.
 * \param threshold for overlapping.
 * \return a list of filtered bboxes.
 */
std::vector<BBox> NonMaximumSuppression(std::vector<BBox> &bboxes,
                                        float threshold);

/*! Create a bounding box from raw inputs.
 * \param pred a list of predictions
 * \param num_classes number of classes to be classified
 * \param loc localisations
 * \param anchor a 4 tuple with reference anchors
 * \param prior_scaling a prior setting for scaling
 * \return a initialised BBox object
 */
BBox CreateBBox(float *pred, int num_classes, float loc[NUM_REFS],
                float anchor[NUM_REFS], float prior_scaling[NUM_REFS]);
}  // namespace libssd

#endif
