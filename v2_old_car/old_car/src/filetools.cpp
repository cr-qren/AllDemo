#include "filetools.hpp"

void file_size(char *file_name, unsigned long &size)
{
  FILE *fp = fopen(file_name, "rb");
  if(fp == NULL)
  {
    perror("fopen");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp),
  fseek(fp, 0, SEEK_SET);

  fclose(fp);
}
vector<string> pic_list(char *file_name)
{
#if 0
  FILE *fp = fopen(file_name, "r");
  if(fp == NULL)
  {
    perror("fopen");
    exit(1);
  }
  char buffer[FILE_NAME_SIZE];
  fread(fp,)
#endif
  char file[FILE_NAME_SIZE];
  sprintf(file, "%s/list.txt", file_name);
  vector<string> img_list;
  ifstream fid(file);
  string tmp;
  while (!fid.eof()) {
    fid >> tmp;
    img_list.push_back(tmp);
  }
  fid.close();
  return img_list;
}
