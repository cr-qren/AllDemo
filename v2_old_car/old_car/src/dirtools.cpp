#include "dirtools.hpp"

int count_files(char *dir_name)
{
  int count = 0;
  DIR *dir;
  struct dirent *ptr;

  dir = opendir(dir_name);
  if(dir == NULL)
  {
    perror("opendir");
    exit(1);
  }
  while((ptr =readdir(dir)) != NULL)
  {
    if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
    {
      continue;
    }
    else
    {
      count++;
    }
  }
  closedir(dir);

  return count;
}
#if 0
vector<string> count_files(char *dir_name)
{
  int count = 0;
  DIR *dir;
  struct dirent *ptr;
  vector<string> v;

  dir = opendir(dir_name);
  if(dir == NULL)
  {
    perror("opendir");
    exit(1);
  }
  while((ptr =readdir(dir)) != NULL)
  {
    if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
    {
      continue;
    }
    else
    {
      v.push_back(ptr->d_name);
      cout << v.end() << endl;
    }
  }
  closedir(dir);

  return v;
}
#endif
