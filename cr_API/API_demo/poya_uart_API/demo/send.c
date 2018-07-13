#include "../serial/serial.h"
#include "../poyaData/poyaData.h"

int main(int argc, char *argv[])
{
  POYA_DATA poyaData;

  poyaData.startCode = 0x02;

  int i;
  for(i = 0; i < TIME_STAMP_BYTE_NUM; i++)
  {
    poyaData.dataStruct.timeStamp[i] = 0x33;
  }
  poyaData.dataStruct.facesCount = 4;

  for(i = 0; i < MAX_FACES_COUNT; i++)
  {
    if(i < 4)
    {
      poyaData.dataStruct.facesInfo[i].classId = 1.0;
      poyaData.dataStruct.facesInfo[i].scoreId = 1.0;
      poyaData.dataStruct.facesInfo[i].minX = 0.4;
      poyaData.dataStruct.facesInfo[i].minY = 0.4;
      poyaData.dataStruct.facesInfo[i].maxX = 0.6;
      poyaData.dataStruct.facesInfo[i].maxY = 0.6;
    }
    else
    {
      poyaData.dataStruct.facesInfo[i].classId = 0;
      poyaData.dataStruct.facesInfo[i].scoreId = 0;
      poyaData.dataStruct.facesInfo[i].minX = 0;
      poyaData.dataStruct.facesInfo[i].minY = 0;
      poyaData.dataStruct.facesInfo[i].maxX = 0;
      poyaData.dataStruct.facesInfo[i].maxY = 0;
    }
  }

  poyaData.stopCode = 0x03;

  int fd = serialOpen("/dev/ttyAMA0", 115200);
  if(fd < 0)
  {
    perror("open serial");
  }

  int n = 1;
  while(n--)
  {
    send_data(fd, poyaData);
  }

  serialClose(fd);
  return 0;
}

