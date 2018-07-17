#include "../cr_tcp/cr_tcp.h"
#include "../yuvData/YUV_transer.h"

int main(int argc, char *argv[])
{
  Cr_tcp so("192.168.123.3", 2345, CLIENT);

  YUV_DATA yuvData;
  YUV_PRAM yuvPram;

  yuvPram.Id       = 4;
  yuvPram.Width    = 1920;
  yuvPram.Height   = 1080;
  yuvPram.Channel  = 3;
  yuvPram.BuffSize = 1920*1080*3;

  yuvData.yuvBuff = alloc_yuv_mem(yuvPram.BuffSize);

  send_yuv((void *)&so, &yuvData);
  free_yuv_mem(yuvData.yuvBuff);
  return 0;
}
