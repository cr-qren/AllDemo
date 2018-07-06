#include "cr_tcp.h"

#ifndef IMG_TRANSER_H
#define IMG_TRANSER_H

typedef struct img_pram{
  int Width;
  int Height;
  int Lenght;
}IMG_PRAM;

typedef struct imgs_data{
 int PramCount;
 IMG_PRAM *Pram;
 int ImgsSize;
 unsigned char *Imgs;
}IMGS_DATA;


IMG_PRAM * alloc_pram_mem(int size);

unsigned char * alloc_imgs_mem(int size);

void send_imgs(void *Tcp, IMGS_DATA *ImgsData);

void recv_imgs(void *Tcp, IMGS_DATA *ImgsData);

void free_pram_mem(IMG_PRAM *ImgPram);

void free_imgs_mem(unsigned char *Imgs);


#endif /*end img_transer_h*/
