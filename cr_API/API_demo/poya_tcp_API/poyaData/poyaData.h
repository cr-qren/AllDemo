#ifndef POYADATA_H
#define POYADATA_H

#ifdef _cplusplus
extern "C" {
#endif

#include <stdio.h>

#ifdef _cplusplus
}
#endif

#include "../cr_tcp/cr_tcp.h"

#define TIME_STAMP_BYTE_NUM 16
#define MAX_FACES_COUNT 20

typedef struct faces_info
{
  float classId;
  float scoreId;
  float minX;
  float minY;
  float maxX;
  float maxY;
}FACES_INFO;

typedef struct data_struct
{
  unsigned char timeStamp[TIME_STAMP_BYTE_NUM];
  unsigned int facesCount;
  FACES_INFO facesInfo[MAX_FACES_COUNT];
}DATA_STRUCT;

typedef struct poya_data
{
  unsigned char startCode;
  DATA_STRUCT dataStruct;
  unsigned char stopCode;
}POYA_DATA;

#ifdef _cplusplus
extern "C" {
#endif

extern void send_data(Cr_tcp *tcpSocket, POYA_DATA poyaData);
extern POYA_DATA recv_data(Cr_tcp *tcpSocket);
extern void print_data(POYA_DATA poyaData);

#ifdef _cplusplus
}
#endif

#endif
