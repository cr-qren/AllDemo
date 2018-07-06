#include "tcpsocket.hpp"

TcpSocket::TcpSocket(char *ip, int port, bool server_flag)
{
  struct sockaddr_in addr;

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  this->server_flag = server_flag;

  if(this->server_flag)
  {
    this->socket_fp = socket(AF_INET, SOCK_STREAM, 0);
    if(this->socket_fp < 0)
    {
      perror("socket");
      exit(1);
    }

    int on = 1;
    setsockopt(socket_fp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    int ret = bind(socket_fp, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if(ret < 0)
    {
      perror("bind");
      exit(1);
    }
    ret = listen(socket_fp, 20);
    if(ret < 0)
    {
      perror("listen");
      exit(1);
    }
  }
  else
  {
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
}

TcpSocket::~TcpSocket()
{
  close(this->socket_fp);
}

void TcpSocket::setconnect_fp(void)
{
  struct sockaddr_in addr;
  socklen_t addr_length = sizeof(addr);
  this->connect_fp = accept(this->socket_fp, (struct sockaddr *)&addr, &addr_length);
}

int TcpSocket::getconnect_fp(void)
{
  return this->connect_fp;
}

void TcpSocket::closeconnect_fp(void)
{
  shutdown(this->connect_fp, SHUT_WR);
  close(this->connect_fp);
}

int TcpSocket::getsocket_fp(void)
{
  return this->socket_fp;
}
