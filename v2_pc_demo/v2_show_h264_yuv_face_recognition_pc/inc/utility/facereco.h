#ifndef faceReco_h
#define faceReco_h

#include "CrFaceSDK.hpp"
#include "jpgtobin.hpp"
#include "opencv2/opencv.hpp"
#include <deque>
#include <fstream>

using namespace cv;
using namespace std;

#define ID_FLAG true
#define tracker

std::vector<string> str_split(string s,  string c);

FaceSDK::CrFaceSDK *Init();

unordered_map<int, string> faceRecognition(FaceSDK::CrFaceSDK *sdk, FaceSDK::Faces faces2save);

#endif /*end Recognition*/
