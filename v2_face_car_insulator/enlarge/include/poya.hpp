#ifndef INFINOVA_HPP
#define INFINOVA_HPP

typedef struct _FRAME_INFO
{
  int nWidth;
  int nHeight;
  unsigned long frameID;
  unsigned long dataLen;
}FRAME_INFO;


void cam_init(void);
void cam_close(void);
void recv_cam_image(void *ptr, unsigned long &size);

#endif
