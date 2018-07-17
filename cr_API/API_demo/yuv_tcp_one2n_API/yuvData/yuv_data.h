#ifndef YUV_TRANSER_H
#define YUV_TRANSER_H

#include "../cr_tcp_one2n/cr_tcp.h"

typedef struct yuv_pram
{
  int Id;
  int Width;
  int Height;
  int Channel;
  int BuffSize;
}YUV_PRAM;


typedef struct yuv_data
{
  YUV_PRAM *yuvPram;
  unsigned char *yuvBuff;
}YUV_DATA;


YUV_PRAM *alloc_yuvPram_mem(int size);

unsigned char * alloc_yuv_mem(int size);

int send_yuv(void *Tcp, YUV_DATA *YUVData);

int recv_yuv(void *Tcp, YUV_DATA *YUVData);

void free_yuvPram_mem(YUV_PRAM *yuvPram);

void free_yuv_mem(unsigned char *yuv);

#endif /*end if*/
