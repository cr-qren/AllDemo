#include "board.hpp"
#include "tcpsocket.hpp"

static TcpSocket *ptcpsocket;
static bool flag_ready = false;

void board_init(void)
{
  ptcpsocket = new TcpSocket("192.168.123.3", 8000, true);
}

void board_close(void)
{
  delete ptcpsocket;
}

bool get_flag(void)
{
  return flag_ready;
}

unsigned char get_signal(void)
{
  unsigned char signal = 0;
  unsigned long signal_size = sizeof(unsigned long);
  ptcpsocket->recv_data(&signal, signal_size);
  return signal;
}

void send_data_to_board(void *data, unsigned long &size)
{
   ptcpsocket->send_data(data, size);
}

void recv_data_from_board(vector<float> &data, unsigned long &size)
{
  unsigned long signal_size = sizeof(size);
  ptcpsocket->recv_data(&size, signal_size);
  float buffer[signal_size/sizeof(float)];
  ptcpsocket->recv_data(buffer, size);
  for(int i = 0; i < signal_size/sizeof(float); i++)
  {
    data.push_back(buffer[i]);
  }
}


