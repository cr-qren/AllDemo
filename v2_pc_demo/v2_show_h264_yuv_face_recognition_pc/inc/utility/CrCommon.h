#ifndef _CR_COMMON_FLAG
#define _CR_COMMON_FLAG

#include <iostream>
#include "opencv2/opencv.hpp"
#include <glog/logging.h>
#include <pthread.h>

namespace FaceSDK {

struct DeltaDistance{
	float deltaX;
	float deltaY;
};

struct Counts{
	int currentCount;
	int totalCount;
};

struct BoundingBox{
	// x-component of top left coordinate
	float x;
	// y-component of top left coordinate
	float y;
	// width of the box
	float w;
	// height of the box
	float h;
	// score of the box;
	float score;
};

struct PaintTrack {
    BoundingBox bbox;
    int track_id;
};

// //struct Faces{
// 	Counts faceCount;
//     std::vector<cv::Mat> faceImages;
// };

struct FaceGroup{
    int faceID;
    cv::Mat faceImage;
};

struct Faces{
	Counts faceCount;
	std::vector<FaceGroup> faceGroups;
};



}

#endif
