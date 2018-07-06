#include "YUV_transer.h"

YUV_PRAM *alloc_yuvPram_mem(int size)
{
  return (YUV_PRAM *)malloc(sizeof(YUV_PRAM)*size);
}

unsigned char * alloc_yuv_mem(int size)
{
  return (unsigned char *)malloc(sizeof(unsigned char)*size);
}

void send_yuv(void *Tcp, YUV_DATA *YUVData)
{
  Cr_tcp *temp = (Cr_tcp *)Tcp;
  temp->sendData(YUVData->yuvPram, sizeof(YUV_PRAM));

  temp->sendData(YUVData->yuvBuff, YUVData->yuvPram->BuffSize*sizeof(unsigned char));
}

void recv_yuv(void *Tcp,  YUV_DATA *YUVData)
{
  Cr_tcp *temp = (Cr_tcp *)Tcp;

  YUVData->yuvPram = alloc_yuvPram_mem(1);
  temp->recvData(YUVData->yuvPram, sizeof(YUV_PRAM));

  YUVData->yuvBuff = alloc_yuv_mem(YUVData->yuvPram->BuffSize);
  temp->recvData(YUVData->yuvBuff, YUVData->yuvPram->BuffSize*sizeof(unsigned char));
}

void free_yuvPram_mem(YUV_PRAM *yuvPram)
{
  free(yuvPram);
}

void free_yuv_mem(unsigned char *yuv)
{
  free(yuv);
}
