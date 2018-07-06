#include "tcprasp.hpp"

TcpRasp::TcpRasp(char *ip, int port)
{
  struct sockaddr_in addr;

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  this->socket_fp = socket(AF_INET, SOCK_STREAM, 0);
  if(this->socket_fp < 0)
  {
    perror("socket");
    exit(1);
  }

  int ret = ::connect(socket_fp, (struct sockaddr *)&addr, sizeof(struct sockaddr));
  if(ret < 0)
  {
    perror("connect");
    exit(1);
  }
}

TcpRasp::~TcpRasp()
{
  ::close(this->socket_fp);
}

void TcpRasp::send_file(char *ptr, char *path)
{
  cout <<" p 0" <<endl;
  char file_name[FILE_NAME_SIZE];
  sprintf(file_name, "%s/%s", path, ptr);
  FILE *fp = fopen(file_name, "rb");
  if(fp == NULL)
  {
    perror("fopen send");
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  unsigned long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char buffer[size];

  int ret = fread(buffer, sizeof(unsigned char), size, fp);
  if(ret < 0)
  {
    perror("fread");
  }

  fclose(fp);
  cout <<" p 1" <<endl;
  ret = send(this->socket_fp, ptr, FILE_NAME_SIZE, MSG_MORE);
  if(ret < 0)
  {
    perror("send file name");
  }
  cout <<" p 2" <<endl;
  ret = send(this->socket_fp, &size, sizeof(unsigned long), 0);
  if(ret < 0)
  {
    perror("send file size");
  }
  cout <<" p 3" <<endl;
  ret = send(this->socket_fp, buffer, size, 0);
  if(ret < 0)
  {
    perror("send file");
  }
}
void TcpRasp::recv_file(char *ptr,char *path)
{
  cout << "read file 1" <<endl;
  int ret = read(this->socket_fp, ptr, FILE_NAME_SIZE);
  if(ret < 0)
  {
    perror("read file name");
  }

  cout << "read file 2" <<endl;
  unsigned long size;
  ret = read(this->socket_fp, &size, sizeof(unsigned long));
  if(ret < 0)
  {
    perror("read file size");
  }

  cout << "read file 3" <<endl;
  unsigned char buffer[size];
  ret = read(this->socket_fp, buffer, size);
  if(ret < 0)
  {
    perror("read file");
  }

  cout << "read file 4" <<endl;
  char file_name[FILE_NAME_SIZE];
  sprintf(file_name, "%s/%s", path, ptr);
  FILE *fp = fopen(file_name, "wb");
  if(fp == NULL)
  {
    perror("fopen read");
    exit(1);
  }

  ret = fwrite(buffer, sizeof(unsigned char), size, fp);
  if(ret < 0)
  {
    perror("fwrite");
  }
  fclose(fp);
  cout << "read file 5" <<endl;
}

