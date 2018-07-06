#ifndef IMG_PROC_H
#define IMG_PROC_H

#include "opencv2/opencv.hpp"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QString>
#include <QRectF>
#include <string>

/*RGB QImage::Format_RGB888 and need to .rgbSwapped()*/
QImage RGB_Mat2RGB_QImage(cv::Mat img);

cv::Mat YUV420Img2RGB_Mat(unsigned char *YUVBuffer, int Height, int Width);

unsigned char *RGB_Mat2YUV420Img(cv::Mat rgbMat);

void imageMosaic(QImage *dstImg, cv::Mat img, std::string name, int x_position, int y_position, int MiniWidth, int MiniHeight);

#endif
