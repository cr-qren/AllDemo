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
  void send_data(Type *ptr, unsigned long size);
  template <typename Type>
  void recv_data(Type *ptr, unsigned long &size);

  void send_file(char *filename);
  void recv_file(char *filename);
private:
  bool server_flag;
  int socket_fp;
  int connect_fp;
};
#endif
