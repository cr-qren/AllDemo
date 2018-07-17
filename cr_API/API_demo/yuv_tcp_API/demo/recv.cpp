#include "../cr_tcp/cr_tcp.h"
#include "../yuvData/YUV_transer.h"

int main(int argc, char *argv[])
{
  Cr_tcp so("192.168.2.3", 2345, CLIENT);

  while(1)
  {
  YUV_DATA yuvData;
  recv_yuv((void *)&so, &yuvData);

  printf("Id      :%d \n",yuvData.yuvPram->Id       );
  printf("Width   :%d \n",yuvData.yuvPram->Width    );
  printf("Height  :%d \n",yuvData.yuvPram->Height   );
  printf("Channel :%d \n",yuvData.yuvPram->Channel  );
  printf("BuffSize:%d \n",yuvData.yuvPram->BuffSize );

  free_yuvPram_mem(yuvData.yuvPram);
  free_yuv_mem(yuvData.yuvBuff);
  }
  return 0;
}
