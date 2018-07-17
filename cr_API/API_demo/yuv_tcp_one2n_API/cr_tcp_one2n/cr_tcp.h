#ifndef TCP_API_H
#define TCP_API_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/socket.h>


#define CLIENT false
#define SERVER true

class Cr_tcp
{
public:
  Cr_tcp(char *ip, int port, bool server_flag);
  ~Cr_tcp();

private:
  char *ip;
  int port;
  bool server_flag;
  bool connect_flag;
  int socket_fd;
  int connect_fd;

public:
  bool isServer(void);
  bool isConnect(void);
  int acceptConn(void);
  int sendData(void *ptr, int size);
  int recvData(void *ptr, int size);

};

#endif /* tcp_api header end */
