/**\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef LIBSSD_POST_PROCESS_HH
#define LIBSSD_POST_PROCESS_HH

#include "bbox.hh"
#include <stdexcept>
#include <vector>

namespace libssd {

// TODO: add a multi-step post-processing algorithm.

/*! The post-processing function.

 * \param preds a list of predictions with shape (feat_size, num_classes)
 * \param locs a list of localisation values with shape (feat_size, NUM_REFS)
 * \param pos_anchors a list of position anchors with shape (feat_size, 2)
 * \param shape_anchors a list of shape anchors with shape (num_anchors, 2)
 * \param num_classes number of prediction classes
 * \param num_layers number of layers
 * \param num_anchors number of anchors
 * \param feat_sizes number of features
 * \param prior_scaling scaling factor for all anchors and localisations
 * \param nms_threshold threshold for NMS
 * \param select_threshold threshold for scoring based selection
 * \return a list of created bboxes
 */
std::vector<BBox> PostProcess(float *preds, float *locs, float *pos_anchors,
                              float *shape_anchors, int num_classes,
                              int num_layers, int *num_anchors, int *feat_sizes,
                              float prior_scaling[NUM_REFS],
                              float nms_threshold, float select_threshold);
}
#endif
