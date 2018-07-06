/**\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef LIBSSD_PRE_PROCESS_HH
#define LIBSSD_PRE_PROCESS_HH

namespace libssd {
/*! The pre_process function for SSD
turn image file with JPG format to pre_processed binary file
pre_process_float : float data binary file
pre_process_fixed : fixed point data binary file
* \param filename : path to find image
*/
void pre_process_float(const char* input_img, char* tmp_img_bin);
void pre_process_fixed(const char* input_img, char* tmp_img_bin);
}  // namespace libssd
#endif
