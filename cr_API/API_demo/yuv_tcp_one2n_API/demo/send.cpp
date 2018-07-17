#include "../cr_tcp_one2n/cr_tcp.h"
#include "../yuvData/yuv_data.h"

int main(int argc, char *argv[])
{
  Cr_tcp so("192.168.2.3", 2345, SERVER);
  while(!so.isConnect())
  {
    so.acceptConn();
    while(1)
    {
      YUV_PRAM yuvPram;
      yuvPram.Id       = 4;
      yuvPram.Width    = 1920;
      yuvPram.Height   = 1080;
      yuvPram.Channel  = 3;
      yuvPram.BuffSize = 1920*1080*3;

      YUV_DATA yuvData;
      yuvData.yuvPram = &yuvPram;
      yuvData.yuvBuff = alloc_yuv_mem(yuvPram.BuffSize);

      int ret = send_yuv((void *)&so, &yuvData);
      free_yuv_mem(yuvData.yuvBuff);
      if(ret < 0)
      {
        fprintf(stderr, "[ Server ] tcp socket break!\n");
        break;
      }
    }
  }
  return 0;
}
