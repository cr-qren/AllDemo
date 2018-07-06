#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

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

#define IP_SIZE 512

class TcpSocket
{
public:
  TcpSocket(char *ip, int port, bool server_flag);
  ~TcpSocket();

  template <typename Type>
    int send_data(Type *ptr, unsigned long size)
    {
      int ret;
      if(this->server_flag)
      {
        ret = send(this->connect_fp, ptr, size, 0);
        if(ret < 0)
        {
          perror("[RASPBERRY] server send");
        }
#if 0
        shutdown(this->connect_fp, SHUT_WR);
        close(this->connect_fp);
#endif
      }
      else
      {
        ret = send(this->socket_fp, ptr, size, 0);
        if(ret < 0)
        {
          perror("[RASPBERRY] client send");
        }
      }
      return ret;
    }

  template <typename Type>
    int recv_data(Type *ptr, unsigned long &size)
    {
      int ret;
      if(this->server_flag)
      {
        ret = recv(this->connect_fp, ptr, size, MSG_WAITALL);
        if(ret < 0)
        {
          perror("[RASPBERRY] server recv");
        }
#if 0
        shutdown(this->connect_fp, SHUT_WR);
        close(this->connect_fp);
#endif
      }
      else
      {
        ret = recv(this->socket_fp, ptr, size, MSG_WAITALL);
        if(ret < 0)
        {
          perror("[RASPBERRY] client recv");
        }
      }
      return ret;
    }
  void setconnect_fp(void);
  int getconnect_fp(void);
  void closeconnect_fp(void);
  int getsocket_fp(void);
private:
  bool server_flag;
  int socket_fp;
  int connect_fp;
};

#endif
