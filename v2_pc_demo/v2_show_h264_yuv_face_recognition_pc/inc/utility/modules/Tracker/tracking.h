#ifndef TRACKING_H
#define TRACKING_H

#include <algorithm>
#include <iostream>
#include <assert.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Tracker/feature.h"
#include "CrCommon.h"

using namespace std;
using namespace cv;

namespace FaceSDK{

// threshold to return face
const float FACE_RETURN_THRESHOLD = 0.9;

//detection-box's confidence to start traking, or get feature
const float SIGMA_H = 0.5;
//two box iou to judge if two box is one object
const float SIGMA_IOU = 0.2;

// input image width and height
const float WIDTH = 1920.0;
const float HEIGHT = 1080.0;

// keep tracking with padding box in MIN_INTERVAL, hold lost traking in MAX_INTERVAL
const int MIN_INTERVAL = 2;
const int MAX_INTERVAL = 20;

// Setting max tracking time for face return
const int MAX_TRACKING_TIME = 200;

// when box moving out of these coordinate, stop tracking
const float MIN_X_COODERNATE = 30.0;
const float MIN_Y_COODERNATE = 30.0;
const float MAX_X_COODERNATE = 1000.0;
const float MAX_Y_COORERNATE = 900.0;

// the similarity of two box
const float SIMILARITY_THRESHOLD = 0.28;

// box's max moving speed
const int MAX_MOVING_SPEED = 60;

// Abandon tracking when the total tacking time is less then the threshold
const int ABANDON_TRACKING_SPAN = 3 + MIN_INTERVAL;


//struct BoundingBox{
//	// x-component of top left coordinate
//	float x;
//	// y-component of top left coordinate
//	float y;
//	// width of the box
//	float w;
//	// height of the box
//	float h;
//	// score of the box;
//	float score;
//};
//
//struct DeltaDistance{
//	float deltaX;
//	float deltaY;
//};
//
//struct Counts{
//	int currentCount;
//	int totalCount;
//};
//
//struct Faces{
//	Counts faceCount;
//	vector<Mat> faceImages;
//};


struct Track{
	vector<BoundingBox> boxes;
    float faceScore;
    Mat faceImage;
	int startFrame;
	int id;
	int skipCount;
	DeltaDistance deltaDistance;
	vector<float> feature;
	float similarity;
	int movingSpeed;
	float centerX;
	float centerY;
	int match_id;
    int trackingTimeCnt;
};

class Tracker{
public:
    Tracker();
    vector<Track> get_active_tracks();
    Faces tracking(deque<int>& frameID, deque<Mat>& frameList, deque<vector<BoundingBox>>& rectList, const int& rejectID);
    void reset_person_count();
    int get_current_tracking_frame_id();
    void reset_track_by_id(int id);

private:
    float sigmaH;
    float sigmaIou;
    int trackId;
    int trackedFrame;
    int trackingFrameID;
    Counts counts;
    vector<Track> lostTracks;
    vector<Track> activeTracks;
    vector<Track> activeLostingTracks;
    inline float intersection_over_union(BoundingBox box1, BoundingBox box2);
    inline int highest_iou(BoundingBox box, vector<BoundingBox> boxes);
    float compare_distance_similarity(Track track, BoundingBox box);
    Faces tracking_in_realtime(const int& frame,  Mat& srcImage, vector<BoundingBox>& frameBoxes, const int& rejectID);

};

}

#endif
