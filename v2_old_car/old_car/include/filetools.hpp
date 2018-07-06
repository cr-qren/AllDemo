#ifndef FILETOOLS_HPP
#define FILETOOLS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#define FILE_NAME_SIZE 512
using namespace std;

template<typename Type>
void dump_file(char *file_name, Type *buffer, unsigned long size)
{
  FILE *fp = fopen(file_name, "wb");
  if(fp == NULL)
  {
    perror("fopen");
    exit(1);
  }

  int ret = fwrite(buffer, sizeof(Type), size/sizeof(Type), fp);
  if(ret < 0)
  {
    perror("fwrite");
    fclose(fp);
    exit(1);
  }

  fclose(fp);
}

template<typename Type>
void load_file(char *file_name, Type *&buffer, unsigned long &size)
{
  FILE *fp = fopen(file_name, "rb");
  if(fp == NULL)
  {
    perror("fopen");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  buffer = new Type[size/sizeof(Type)];

  int ret = fread(buffer, sizeof(Type), size/sizeof(Type), fp);
  if(ret < 0)
  {
    perror("fread");
    fclose(fp);
    exit(1);
  }

  fclose(fp);
}

void file_size(char *file_name,unsigned long &size);

vector<string> pic_list(char *file_name);

#endif
