#ifndef PICTOOLS_HPP
#define PICTOOLS_HPP

#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QColor>

#define RECT_ARG_NUM 5

#define PICTURE_WIDTH 1920
#define PICTURE_HEIGHT 1080

void draw_rect(QImage *img, float *buffer, unsigned long size);

#endif
