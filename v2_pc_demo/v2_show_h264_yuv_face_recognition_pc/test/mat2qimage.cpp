#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QApplication>
#include <QString>
#include <QRectF>
#include <string>


/*RGB QImage::Format_RGB888 and need to .rgbSwapped()*/
QImage RGB_Mat2RGB_QImage(cv::Mat img)
{
  return QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped().copy();
}


cv::Mat YUV420Img2RGB_Mat(unsigned char *YUVBuffer, int Height, int Width)
{
  cv::Mat rgbMat;
  cv::Mat YUV420Img(Height+Height/2, Width, CV_8UC1, YUVBuffer);
  std::cout<<"asdasdasd3:"<<std::endl;
  cv::cvtColor(YUV420Img, rgbMat, CV_YUV2BGR_I420);
  //cv::cvtColor(YUV420Img, rgbMat, cv::COLOR_YUV2RGB_IYUV);
  return rgbMat;
}

unsigned char *RGB_Mat2YUV420Img(cv::Mat rgbMat)
{
  cv::Mat YUV420Img;
  //cv::cvtColor(rgbMat, YUV420Img, COLOR_YUV2RGB_I420);
  cv::cvtColor(rgbMat, YUV420Img, cv::COLOR_YUV2RGB_IYUV);
  unsigned char *YUVBuffer =(unsigned char *)malloc(rgbMat.cols * rgbMat.rows * 3 /2);
  memcpy(YUVBuffer, YUV420Img.data, rgbMat.cols * rgbMat.rows * 3 /2);
  return YUVBuffer;
}


void imageMosaic(QImage *dstImg, cv::Mat img, std::string name, int x_position, int y_position, int MiniWidth, int MiniHeight)
{
  cv::Mat re_img;
  cv::resize(img, re_img, cv::Size(MiniWidth, MiniHeight*2/3));
  QImage srcImg = QImage(re_img.data, re_img.cols, re_img.rows, re_img.step, QImage::Format_RGB888).rgbSwapped().copy();

  QImage miniImg = QImage(MiniWidth, MiniHeight, QImage::Format_RGB888).rgbSwapped();
  miniImg.fill(qRgb(255, 255, 255));
  QPainter paint_1(&miniImg);
  paint_1.drawImage(QRectF(0, 0, MiniWidth, MiniHeight*2/3), srcImg);
  paint_1.end();

  QPainter paint_2(&miniImg);
  paint_2.setPen(QColor(0, 128, 255));
  paint_2.setFont(QFont("Arial", 10));

  QString str = QString::fromStdString(name);
  paint_2.drawText(QRectF(0, MiniHeight*2/3, MiniWidth, MiniHeight/3), Qt::AlignCenter, str);
  paint_2.end();

  QPainter paint_3(dstImg);
  paint_3.drawImage(QRectF(x_position*MiniWidth, y_position*MiniHeight, MiniWidth, MiniHeight), miniImg);
  paint_3.end();
}




int main(int argc, char *argv[])
{
  std::cout<<"asdasdasd1:"<<std::endl;
  FILE* fp = fopen("../img_dataset/yuv.bin", "rb");
  fseek(fp, SEEK_END, 0);
  int size = ftell(fp);
  fseek(fp, SEEK_SET, 0);
  std::cout<<"asdasdasd2:"<<std::endl;
  unsigned char *yuvbuff = (unsigned char *)malloc(sizeof(unsigned char)*size);
  int n = fread(yuvbuff, sizeof(unsigned char), size, fp);
  fclose(fp);
  cv::Mat xx = YUV420Img2RGB_Mat(yuvbuff, 1080, 1920);
  imwrite("hhh.jpg", xx);
}



//int main(int argc, char *argv[])
//{
//  QApplication app(argc, argv);
//  cv::Mat img_1 = cv::imread("../img_dataset/faces2save.jpg");
//  cv::Mat img_2 = cv::imread("../img_dataset/faces2save.jpg");
//
//  QImage img = QImage(1920*0.25, 1080*0.5, QImage::Format_RGB888).rgbSwapped();
//  img.fill(qRgb(255, 100, 0));
//
//  int n = 5;
//  while(n--)
//  {
//    imageMosaic(&img, img_1, "xiongchao", 0, n, img.width()/5, img.height()/5);
//  }
//
//  img.save("hh.jpg", "JPG");
//
//  return 0;
//}

//int main(int argc, char *argv[])
//{
//  cv::Mat img = cv::imread("../img_dataset/faces2save.jpg");
//  QImage img_re = RGB_Mat2RGB_QImage(img);
//  img_re.save("re_img.jpg","JPG");
//}


