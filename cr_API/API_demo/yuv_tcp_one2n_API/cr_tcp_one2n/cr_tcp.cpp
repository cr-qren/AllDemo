#include "cr_tcp.h"

/*
 * init to socket.
 *
 * ip         : Server IP.
 * port       : Proccess PORT.
 * server_flag: SERVER Or CLIENT flag.
 *********************************************
 */

Cr_tcp::Cr_tcp(char *ip, int port, bool server_flag)
{
  this->ip = (char *)malloc(sizeof(char)*256);

  strcpy(this->ip, ip);
  this->port = port;
  this->server_flag = server_flag;
  this->connect_flag = false;

  if(this->server_flag)
  {
    //Server init.
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(this->socket_fd < 0)
    {
      perror("[ TCP Server ] Socket");
      exit(1);
    }

    int on = 1;
    int ret = setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(ret < 0)
    {
      perror("[ TCP Server ] SetSockOpt");
      exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip) ;

    ret = bind(this->socket_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if(ret < 0)
    {
      perror("[ TCP Server ] Bind");
      exit(1);
    }

    ret = listen(this->socket_fd, 20);
    if(ret < 0)
    {
      perror("[ TCP Server ] Listen");
      exit(1);
    }
  }
  else
  {
    //Client init.
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(this->socket_fd < 0)
    {
      perror("[ TCP Client ] Socket");
      exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int times = 5;
    int ret = connect(this->socket_fd,  (struct sockaddr *)&addr, sizeof(struct sockaddr));
    while(ret < 0)
    {
      this->connect_flag = false;
      if(!times--)
      {
        exit(1);
      }
      printf("[ TCP Client ] Connect the %d times !\n", times);
      perror("[ TCP Client ] Connect");
      ret = connect(this->socket_fd,  (struct sockaddr *)&addr, sizeof(struct sockaddr));
    }
    this->connect_flag = true;
  }
}

/*
 * TCP doing when deleted.
 ***************************
 */

Cr_tcp::~Cr_tcp()
{
  //Free ip buffer.
  free(this->ip);

  //close socket_fd or connect_fd.
  if(this->server_flag)
  {
    close(this->connect_fd);
  }
  close(this->socket_fd);
}

/*
 * Check the server flag.
 ***************************
 */

bool Cr_tcp::isServer()
{
  if(this->server_flag)
  {
    return true;
  }
  return false;
}

/*
 * Check the connect flag.
 ***************************
 */

bool Cr_tcp::isConnect()
{
  if(this->connect_flag)
  {
    return true;
  }
  return false;
}

/*
 * As the Server platform for TCP accept connect.
 **************************************************
 */

int Cr_tcp::acceptConn()
{
  struct sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);

  this->connect_fd = accept(this->socket_fd, (struct sockaddr *)&client_addr, &addr_len);
  if(this->connect_fd < 0)
  {
    perror("[ TCP Server ] Accept");
    this->connect_flag = false;
  }
  this->connect_flag = true;

  return this->connect_fd;
}

/*
 * TCP send data about Server or Client.
 * ptr  : the buffer or struct addr.
 * size : the buffer or struct size.
 **********************************************
 */
int Cr_tcp::sendData(void *ptr, int size)
{
  int ret = 0;

  if(this->server_flag)
  {
    ret = send(this->connect_fd, ptr, size, MSG_NOSIGNAL);
    if(ret < 0)
    {
      perror("[ TCP Server ] Socketfd Break And Send");
      this->connect_flag = false;
    }
  }
  else
  {
    ret = send(this->socket_fd, ptr, size, MSG_NOSIGNAL);
    if(ret < 0)
    {
      perror("[ TCP Client ] Socketfd Break And Send");
      this->connect_flag = false;
    }
  }

  return ret;
}

/*
 * TCP recv data about Server or Client.
 * ptr  : the buffer or struct addr.
 * size : the buffer or struct size.
 **********************************************
 */
int Cr_tcp::recvData(void *ptr, int size)
{
  int ret = 0;

  if(this->server_flag)
  {
    ret = recv(this->connect_fd, ptr, size, MSG_WAITALL);
    if(ret <= 0)
    {
      perror("[ TCP Server ] Socketfd Break And Recv");
      this->connect_flag = false;
    }
  }
  else
  {
    ret = recv(this->socket_fd, ptr, size, MSG_WAITALL);
    if(ret <= 0)
    {
      perror("[ TCP Client ] Socketfd Break And Recv");
      this->connect_flag = false;
    }
  }

  return ret 
}
