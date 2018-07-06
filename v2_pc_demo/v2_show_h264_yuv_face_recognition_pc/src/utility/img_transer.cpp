#include "img_transer.h"

IMG_PRAM * alloc_pram_mem(int size)
{
  return (IMG_PRAM *)malloc(sizeof(IMG_PRAM)*size);
}

unsigned char * alloc_imgs_mem(int size)
{
  return (unsigned char *)malloc(sizeof(unsigned char)*size);
}

void send_imgs(void *Tcp, IMGS_DATA *ImgsData)
{
  Cr_tcp *temp_tcp = (Cr_tcp *)Tcp;

  temp_tcp->sendData(&ImgsData->PramCount, sizeof(ImgsData->PramCount));

  temp_tcp->sendData(ImgsData->Pram, ImgsData->PramCount * (sizeof(IMG_PRAM)));

  temp_tcp->sendData(&ImgsData->ImgsSize, sizeof(ImgsData->ImgsSize));

  temp_tcp->sendData(ImgsData->Imgs, ImgsData->ImgsSize);
}

void recv_imgs(void *Tcp, IMGS_DATA *ImgsData)
{
  Cr_tcp *temp_tcp = (Cr_tcp *)Tcp;

  temp_tcp->recvData(&ImgsData->PramCount, sizeof(ImgsData->PramCount));

  ImgsData->Pram = alloc_pram_mem(ImgsData->PramCount);

  temp_tcp->recvData(ImgsData->Pram, ImgsData->PramCount * (sizeof(IMG_PRAM)));

  temp_tcp->recvData(&ImgsData->ImgsSize, sizeof(ImgsData->ImgsSize));

  ImgsData->Imgs = alloc_imgs_mem(ImgsData->ImgsSize);

  temp_tcp->recvData(ImgsData->Imgs, ImgsData->ImgsSize);
}

void free_pram_mem(IMG_PRAM *ImgPram)
{
  free(ImgPram);
}

void free_imgs_mem(unsigned char *Imgs)
{
  free(Imgs);
}
