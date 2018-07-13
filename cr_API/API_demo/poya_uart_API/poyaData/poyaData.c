#ifdef _cplusplus
extern "C" {
#endif

#include "poyaData.h"

#ifdef _cplusplus
}
#endif

void send_data(int fd, POYA_DATA poyaData)
{
  serialPutchar(fd, poyaData.startCode);
  serialPutString(fd, poyaData.dataStruct.timeStamp, TIME_STAMP_BYTE_NUM);
  serialPutString(fd, (unsigned char *)&poyaData.dataStruct.facesCount, sizeof(int));
  int i;
  for(i = 0; i < poyaData.dataStruct.facesCount; i++)
  {
    serialPutString(fd, (unsigned char *)&poyaData.dataStruct.facesInfo[i].classId, sizeof(float));
    serialPutString(fd, (unsigned char *)&poyaData.dataStruct.facesInfo[i].scoreId, sizeof(float));
    serialPutString(fd, (unsigned char *)&poyaData.dataStruct.facesInfo[i].minX, sizeof(float));
    serialPutString(fd, (unsigned char *)&poyaData.dataStruct.facesInfo[i].minY, sizeof(float));
    serialPutString(fd, (unsigned char *)&poyaData.dataStruct.facesInfo[i].maxX, sizeof(float));
    serialPutString(fd, (unsigned char *)&poyaData.dataStruct.facesInfo[i].maxY, sizeof(float));
  }
  serialPutchar(fd, poyaData.stopCode);
}

POYA_DATA recv_data(int fd)
{
  POYA_DATA poyaData;
  if(serialDataAvail(fd) > 0)
  {
    poyaData.startCode = serialGetchar(fd);
    serialGetString(fd, poyaData.dataStruct.timeStamp, TIME_STAMP_BYTE_NUM);
    serialGetString(fd, (char *)&poyaData.dataStruct.facesCount, sizeof(int));
    int i;
    for(i = 0; i < poyaData.dataStruct.facesCount; i++)
    {
      serialGetString(fd, (char *)&poyaData.dataStruct.facesInfo[i].classId, sizeof(float));
      serialGetString(fd, (char *)&poyaData.dataStruct.facesInfo[i].scoreId, sizeof(float));
      serialGetString(fd, (char *)&poyaData.dataStruct.facesInfo[i].minX, sizeof(float));
      serialGetString(fd, (char *)&poyaData.dataStruct.facesInfo[i].minY, sizeof(float));
      serialGetString(fd, (char *)&poyaData.dataStruct.facesInfo[i].maxX, sizeof(float));
      serialGetString(fd, (char *)&poyaData.dataStruct.facesInfo[i].maxY, sizeof(float));
    }
    poyaData.stopCode = serialGetchar(fd);
  }
  return poyaData;
}

void print_data(POYA_DATA poyaData)
{
  printf("start: 0x%x\n", poyaData.startCode);

  printf("timeStamp:");
  int i;
  for(i = 0; i < TIME_STAMP_BYTE_NUM; i++)
  {
    printf("0x%x ", poyaData.dataStruct.timeStamp[i]);
  }

  printf("\nfacesCount: %d\n", poyaData.dataStruct.facesCount);

  for(i = 0; i < poyaData.dataStruct.facesCount; i++)
  {
    printf("classId: %0.3f\n", poyaData.dataStruct.facesInfo[i].classId);
    printf("scoreId: %0.3f\n", poyaData.dataStruct.facesInfo[i].scoreId);
    printf("minX   : %0.3f\n", poyaData.dataStruct.facesInfo[i].minX   );
    printf("minY   : %0.3f\n", poyaData.dataStruct.facesInfo[i].minY   );
    printf("maxX   : %0.3f\n", poyaData.dataStruct.facesInfo[i].maxX   );
    printf("maxY   : %0.3f\n", poyaData.dataStruct.facesInfo[i].maxY   );
  }

  printf("stop: 0x%x\n", poyaData.stopCode);
}

