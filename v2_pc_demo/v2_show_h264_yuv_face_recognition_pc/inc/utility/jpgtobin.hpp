#ifndef _JPG2BIN_FLAG
#define _JPG2BIN_FLAG

#include<iostream>
#include<opencv2/opencv.hpp>
#include<stdio.h>
using namespace std;
using namespace cv;

void pre_op(char* filename);
void pre_op(char* filename, char* output_filename);
// read frome filename to float* data
void pre_op(char* filename, vector<float>& int_img_ptr);

void pre_op(Mat& temp, vector<float>& int_img_ptr);

#endif
