#ifndef TCPRASP_HPP
#define TCPRASP_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>

using namespace std;

#define FILE_NAME_SIZE 512

class TcpRasp
{
public:
  TcpRasp(char *ip, int port);
  ~TcpRasp();

  template <typename Type>
    void send_data(Type *ptr, unsigned long size)
    {
      int ret = send(this->socket_fp, &size, sizeof(unsigned long), 0);
      if(ret < 0)
      {
        perror("send data size");
      }
      ret = send(this->socket_fp, ptr, size, 0);
      if(ret < 0)
      {
        perror("send data");
      }
    }
  template <typename Type>
    void recv_data(Type *&ptr, unsigned long &size)
    {
      int ret = recv(this->socket_fp, &size, sizeof(unsigned long), 0);
      if(ret < 0)
      {
        perror("recv data size");
      }

      ptr = new Type[size/sizeof(Type)];

      ret = recv(this->socket_fp, ptr, size, 0);
      if(ret < 0)
      {
        perror("recv data");
      }
    }


  void send_file(char *ptr, char *path);
  void recv_file(char *ptr, char *path);

private:
  int socket_fp;
  int connect_fp;
};
#endif
