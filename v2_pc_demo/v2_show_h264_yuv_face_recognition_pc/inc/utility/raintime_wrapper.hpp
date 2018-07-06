#ifndef _raintime_wrapper
#define _raintime_wrapper

#include "plumber_ir/dfg_def.pb.h"

#include <fcntl.h>
#include <glog/logging.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "raintime/core.h"
#include "raintime/nn/accel/accel_cpu.hh"
#include "third_party/libssd/bbox.hh"
#include "third_party/libssd/post_process.hh"
#include "third_party/libssd/pre_process.hh"
#include "jpgtobin.hpp"

#include <string>

#define NUM_EXT_LAYERS 6

namespace raintime_wrapper {

using namespace plumber_ir;
using namespace google::protobuf;
using namespace raintime::dfg;
using namespace raintime::nn::accel::cpu;
using namespace libssd;

class RaintimeUtils {

public:

    RaintimeUtils() {
        runner = new DFGRunner();
    }

    ~RaintimeUtils() {
        delete runner;
        delete dfg_def;
        delete weight_data_map;
        delete builder;
        delete dfg;
    }

    void InitData(string f1, string f2, string f3);
    vector<libssd::BBox> raintime_wrapper(vector<float>& input_data);

private:

    DFGRunner *runner;
    DFGDef *dfg_def;
    DFGDataMap *weight_data_map;
    DFGBuilder *builder;
    DFG *dfg;

    float prior_scaling[libssd::NUM_REFS] = {0.1, 0.1, 0.2, 0.2};
    int num_anchors[NUM_EXT_LAYERS] = {4, 4, 6, 6, 6, 4};
    int feat_sizes[NUM_EXT_LAYERS] = {64 * 64 * 4, 32 * 32 * 4, 16 * 16 * 6,
                                  8 * 8 * 6,   4 * 4 * 6,   2 * 2 * 4};
    vector<float> pos_anchors, shape_anchors;


    DFGDef *LoadFromFile(string filepath);
    DFGDataMap *LoadFromDataDir(string data_dir);
    //DFGDataMap *LoadFromImageFile(const char *image_file);
    DFGDataMap *LoadFromMem(vector<float>& input_tensor);
    std::vector<libssd::BBox> post_op(float *loc, float *cls);
    std::vector<float> read_file_vec(std::string data_dir, std::string file_name);

};
//int raintime_wrapper(DFGRunner* runner, char * pbtxtPath, char * paramPath, char * inputPath, char * dataPath, char *outPath);


}

#endif
