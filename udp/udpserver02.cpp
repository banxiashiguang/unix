#include "common.h"

static int count;
static void print_count(int);

void dg_check_number(int fd,struct sockaddr* client_addr,socklen_t client_len)
{
	int nread,n;
	char recvBuf[MAXLEN];
	signal(SIGINT, print_count);

	n = 220*1024;
	setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));//修改udp接收缓冲区的大小

	while(1)
	{
		memset(recvBuf,0,sizeof(recvBuf));
		nread = recvfrom(fd, recvBuf, MAXLEN, 0, client_addr, &client_len);
		count++;
		cout<<"count="<<count<<endl;
	}
}

static void print_count(int signo)
{
	cout<<"print_count="<<count<<endl;
	exit(0);
}

int main(int argc, char const *argv[])
{
	int server_fd;
	struct sockaddr_in server_addr,client_addr;

	memset(&server_addr,0,sizeof(server_addr));
	memset(&client_addr,0,sizeof(client_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t client_len = sizeof(client_addr);

	server_fd = Socket(AF_INET, SOCK_DGRAM, 0);
	Bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	dg_check_number(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	close(server_fd);
	return 0;
}