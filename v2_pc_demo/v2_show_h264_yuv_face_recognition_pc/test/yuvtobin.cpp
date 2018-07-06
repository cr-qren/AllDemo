
#include "opencv2/opencv.hpp"
cv::Mat YUV420Img2RGB_Mat(unsigned char *YUVBuffer, int Height, int Width)
{
  cv::Mat rgbMat(Height, Width, CV_8UC3);
  cv::Mat YUV420Img(Height+Height/2, Width, CV_8UC1, YUVBuffer);
  cv::cvtColor(YUV420Img, rgbMat, CV_YUV2RGB_YV12);
  return rgbMat;
}

int main(int argc, char *argv[])
{
  FILE* fp = fopen("../img_dataset/yuv.bin", "rb");
  if(!fp)
  {
    perror("error open");
  }
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  unsigned char *yuvbuff = (unsigned char *)malloc(sizeof(unsigned char)*size+1);
  int n = fread(yuvbuff, sizeof(unsigned char), size, fp);
  fclose(fp);
  cv::Mat xx = YUV420Img2RGB_Mat(yuvbuff, 1080, 1920);
  imwrite("hhh.jpg", xx);
}
