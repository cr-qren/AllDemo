#ifdef _cplusplus
extern "C" {
#endif

#include "poyaData.h"

#ifdef _cplusplus
}
#endif

void send_data(Cr_tcp *tcpSocket, POYA_DATA poyaData)
{
  tcpSocket->sendData(&poyaData, sizeof(poyaData));
}

POYA_DATA recv_data(Cr_tcp *tcpSocket)
{
  POYA_DATA poyaData;
  tcpSocket->recvData(&poyaData, sizeof(poyaData));
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

