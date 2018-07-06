#include "poya.hpp"
#include "tcpsocket.hpp"

static TcpSocket *ptcpsocket;

void cam_init(void)
{
  ptcpsocket = new TcpSocket("192.168.1.168", 31003, false);
}

void cam_close(void)
{
  delete ptcpsocket;
}

void recv_cam_image(void *ptr, unsigned long &size)
{

  FRAME_INFO frame_info;

  unsigned long recv_size = sizeof(frame_info);
  ptcpsocket->recv_data(&frame_info, recv_size);

  size = frame_info.dataLen;
  ptcpsocket->recv_data(ptr, size);

}


