# ifndef FEATURE_H
# define FEATURE_H

#include <algorithm>
#include <iostream>
#include <assert.h>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const int ROI_SIZE = 64;
const float PI = 3.14159;

struct TrackVector{
    float x;
    float y;
};

vector<float> extract_roi_hog(Mat& image, const Rect& rect);
vector<float> extract_roi_uniform_lbp(Mat& srcImg, const Rect& rect);
float calculate_included_angle(TrackVector& A, TrackVector& B);
float compare_feature_similarity(const vector<float>& f1, const vector<float>& f2);




# endif
