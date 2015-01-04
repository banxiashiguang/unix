#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
#define SERVER_PORT 9000 
#define QENLEN 5
#define MAXLEN 1024
#define NDG 2000
#define DLEN 1400

int Socket(int,int,int);
void Bind(int,const struct sockaddr *,socklen_t);
void Listen(int,int);
int Accept(int,struct sockaddr*,socklen_t*);
void str_echo(int fd);
void str_cli(int);
void select_str_cli(int);
void shutdown_str_cli(int);
int max(int,int);
void Writen(int fd,void *buf,size_t n);
void Connect(int fd,const struct sockaddr *,socklen_t);

void sig_chld(int signo);
void str_echo_plus(int fd);

//udp服务器处理函数
void dg_echo(int,struct sockaddr*,socklen_t);

//udp客户端处理函数
void dg_client(int,struct sockaddr*,socklen_t);

//udp客户端(检查服务器地址)
void dg_check_client(int,struct sockaddr*,socklen_t);

//udp客户端调用connect函数，提高性能
void dg_connect_client(int,struct sockaddr*,socklen_t);

//udp客户端发送固定数目的数据
void dg_fixed_client(int,struct sockaddr*,socklen_t);

//返回本地套接字关联的本地协议地址
void Getsockname(int,struct sockaddr*,socklen_t*);

struct addrinfo *host_serv(const char *hostname,const char *service,int family,int socktype);

int tcp_connect(const char *hostname,const char *service);

//设置定时器
int connect_timeout(int sockfd,const struct sockaddr* serveraddr,socklen_t serverlen,int nsec);
static void connect_alarm(int signo);

//unix 域套接字
int my_open(const char* pathname,int mode);
int write_fd(int sockfd,void *ptr,size_t nbytes,int sendfd);
int read_fd(int sockfd,void *ptr,size_t nbytes,int *recvfd);