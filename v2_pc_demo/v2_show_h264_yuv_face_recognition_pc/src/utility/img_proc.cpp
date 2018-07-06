#include "img_proc.h"
/*RGB QImage::Format_RGB888 and need to .rgbSwapped()*/
QImage RGB_Mat2RGB_QImage(cv::Mat img)
{
  return QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped().copy();
}

cv::Mat YUV420Img2RGB_Mat(unsigned char *YUVBuffer, int Height, int Width)
{
  cv::Mat rgbMat(Height, Width, CV_8UC3);
  cv::Mat YUV420Img(Height+Height/2, Width, CV_8UC1, YUVBuffer);
  cv::cvtColor(YUV420Img, rgbMat, CV_YUV2RGB_YV12);
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
  //cv::resize(img, re_img, cv::Size(MiniWidth, MiniHeight*2/3));
  if(MiniWidth*1.0/(MiniHeight*2/3) > img.cols*1.0/img.rows)
  {
    cv::resize(img, re_img, cv::Size(img.cols*MiniHeight*2/3/img.rows, MiniHeight*2/3));
  }
  else if(img.cols > img.rows)
  {
    cv::resize(img, re_img, cv::Size(MiniWidth, img.rows*MiniWidth/img.cols));
  }
  else
  {
    cv::resize(img, re_img, cv::Size(img.cols*MiniHeight*2/3/img.rows, MiniHeight*2/3));
  }

  printf("img c,%d r,%d",img.cols,img.rows);
  printf("reimg c,%d r,%d",re_img.cols,re_img.rows);

  QImage srcImg = QImage(re_img.data, re_img.cols, re_img.rows, re_img.step, QImage::Format_RGB888).rgbSwapped().copy();

  QImage miniImg = QImage(MiniWidth, MiniHeight, QImage::Format_RGB888).rgbSwapped();
  miniImg.fill(qRgb(210, 233, 255));
  QPainter paint_1(&miniImg);
  paint_1.drawImage(QRectF(0, 0, srcImg.width(), srcImg.height()), srcImg);
  //paint_1.drawImage(QRectF(0, 0, MiniWidth, MiniHeight*2/3), srcImg);
  paint_1.end();

  QPainter paint_2(&miniImg);
  paint_2.setPen(QColor(0, 102, 204));
  paint_2.setFont(QFont("Arial", 10));

  QString str = QString::fromStdString(name);
  paint_2.drawText(QRectF(0, MiniHeight*2/3, MiniWidth, MiniHeight/3), Qt::AlignLeft, str);
  paint_2.end();

  QPainter paint_3(dstImg);
  paint_3.drawImage(QRectF(x_position*MiniWidth, y_position*MiniHeight, MiniWidth, MiniHeight), miniImg);
  paint_3.end();
}

