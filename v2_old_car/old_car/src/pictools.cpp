#include "pictools.hpp"

void draw_rect(QImage *img, float *buffer, unsigned long size)
{
		QPainter paint(img);
		paint.setBrush(Qt::NoBrush);
		paint.setPen(QPen(QColor(0,255,0),3));

    for(int i = 0; i < size/sizeof(float)/RECT_ARG_NUM; i++)
		{
			paint.drawRect(PICTURE_WIDTH  * buffer[i*RECT_ARG_NUM + 1],
										PICTURE_HEIGHT * buffer[i*RECT_ARG_NUM + 2],
										PICTURE_WIDTH  * (buffer[i*RECT_ARG_NUM + 3] - buffer[i*RECT_ARG_NUM + 1]),
										PICTURE_HEIGHT * (buffer[i*RECT_ARG_NUM + 4] - buffer[i*RECT_ARG_NUM + 2]));
		}
		paint.end();
}

