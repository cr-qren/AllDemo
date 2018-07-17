#include "yuv_data.h"

YUV_PRAM *alloc_yuvPram_mem(int size)
{
  return (YUV_PRAM *)malloc(sizeof(YUV_PRAM)*size);
}

unsigned char * alloc_yuv_mem(int size)
{
  return (unsigned char *)malloc(sizeof(unsigned char)*size);
}

int send_yuv(void *Tcp, YUV_DATA *YUVData)
{
  Cr_tcp *temp = (Cr_tcp *)Tcp;
  int ret;

  ret = temp->sendData(YUVData->yuvPram, sizeof(YUV_PRAM));
  if(ret < 0)
  {
    return ret;
  }

  ret = temp->sendData(YUVData->yuvBuff, YUVData->yuvPram->BuffSize*sizeof(unsigned char));
  if(ret < 0)
  {
    return ret;
  }

  return ret;
}

int recv_yuv(void *Tcp,  YUV_DATA *YUVData)
{
  Cr_tcp *temp = (Cr_tcp *)Tcp;
  int ret;

  YUVData->yuvPram = alloc_yuvPram_mem(1);
  ret = temp->recvData(YUVData->yuvPram, sizeof(YUV_PRAM));
  if(ret < 0)
  {
    free_yuvPram_mem(YUVData->yuvPram);
    return ret;
  }

  YUVData->yuvBuff = alloc_yuv_mem(YUVData->yuvPram->BuffSize);
  ret = temp->recvData(YUVData->yuvBuff, YUVData->yuvPram->BuffSize*sizeof(unsigned char));
  if(ret < 0)
  {
    free_yuv_mem(YUVData->yuvBuff);
    return ret;
  }

  return ret;
}

void free_yuvPram_mem(YUV_PRAM *yuvPram)
{
  free(yuvPram);
}

void free_yuv_mem(unsigned char *yuv)
{
  free(yuv);
}
