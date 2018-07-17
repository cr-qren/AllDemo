#include "../cr_tcp_one2n/cr_tcp.h"
#include "../yuvData/yuv_data.h"

int main(int argc, char *argv[])
{
  Cr_tcp so("192.168.2.3", 2345, CLIENT);

  do{

    while(1)
    {
      int ret;
      YUV_DATA yuvData;
      ret = recv_yuv((void *)&so, &yuvData);
      if(ret < 0)
      {
	fprintf(stderr, "[ Client ] socket break !\n");
        break;
      }

      printf("Id      :%d \n",yuvData.yuvPram->Id       );
      printf("Width   :%d \n",yuvData.yuvPram->Width    );
      printf("Height  :%d \n",yuvData.yuvPram->Height   );
      printf("Channel :%d \n",yuvData.yuvPram->Channel  );
      printf("BuffSize:%d \n",yuvData.yuvPram->BuffSize );

      free_yuvPram_mem(yuvData.yuvPram);
      free_yuv_mem(yuvData.yuvBuff);
    }
    Cr_tcp so("192.168.2.8", 2345, CLIENT);

  }while(!so.isConnect());

  return 0;
}
