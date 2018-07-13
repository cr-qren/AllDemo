#include "../cr_tcp/cr_tcp.h"
#include "../poyaData/poyaData.h"

int main(int argc, char *argv[])
{
  Cr_tcp tcpSocket("192.168.123.3", 1234, SERVER);
  int n = 10;
  while(n--)
  {
    POYA_DATA poyaData = recv_data(&tcpSocket);
    print_data(poyaData);
  }
  return 0;
}
