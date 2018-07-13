#include "../serial/serial.h"
#include "../poyaData/poyaData.h"

int main(int argc, char *argv[])
{
  int fd = serialOpen("/dev/ttyAMA0", 115200);
  int n = 10000;
  while(n--)
  {
    POYA_DATA poyaData = recv_data(fd);
    print_data(poyaData);
    sleep(1);
  }
  serialClose(fd);
  return 0;
}

