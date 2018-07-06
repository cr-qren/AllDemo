#ifndef _CR_DETECT_FLAG
#define _CR_DETECT_FLAG

#include <iostream>
#include "opencv2/opencv.hpp"
#include <deque>
#include "pthread.h"
#include "CrCommon.h"
#include "raintime_wrapper.hpp"

using namespace cv;
using namespace std;

#define Debug false

namespace FaceSDK {

using namespace raintime_wrapper;

class CrDetector {

public:
    CrDetector(pthread_mutex_t& det_mid){
        is_running = false;
        runner = new RaintimeUtils();
        cur_id = 0;
        CrInit(det_mid);
    };

    ~CrDetector(){
        delete runner;
    };
    //void run(deque<int>& frame_id, deque<Mat>& frames, deque<vector<float>>& rects);

    void CrDetetorInit(string s1, string s2, string s3) {
        runner->InitData(s1, s2, s3);
    }

    void run(deque<int>& fid, deque<Mat>& fs, deque<vector<BoundingBox>>& rs, int id, Mat cur_frame);
    vector<BoundingBox> offline_det(Mat& cur_frame);

    pthread_mutex_t *mid;
    pthread_t tid;

    /*ssd_related vars*/
    //DFGRunner* runner;
    //CrFaceSDK related vars
    RaintimeUtils* runner;

    deque<int> *frame_id;
    deque<Mat> *frames;
    deque<vector<BoundingBox>> *rect_results;


    Mat process_frame;
    bool is_running;
    bool data_ready = false;
    int cur_id;


private:
    void CrInit(pthread_mutex_t& det_mid);
};


void* raintime_thread_wrapper(void* data);

}


#endif
