#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main(int argc, char const *argv[])
{
	int client_fd;
	int nread;
	char recv[50];
	struct sockaddr_in server_addr;

	if(argc!=2)
	{
		cout<<"Usage:./daytimecli ip_address"<<endl;
		return -1;
	}

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(13);
	//server_addr.sin_addr.s_addr=inet_addr("206.168.112.96");
	if(inet_pton(AF_INET,argv[1],&server_addr.sin_addr)<=0)
	{
		perror("inet_pton");
		return -1;
	}
	socklen_t addr_len=sizeof(struct sockaddr_in);

	if((client_fd=socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("socket error");
		return -1;
	}
	if(connect(client_fd, (struct sockaddr*)&server_addr, addr_len) < 0)
	{
		perror("connect error");
		return -1;
	}

	memset(recv,0,sizeof(recv));
	while(nread=read(client_fd,recv,sizeof(recv)) > 0)
	{
		string str=recv;
		cout<<str<<endl;
		memset(recv,0,sizeof(recv));
	}
	close(client_fd);
	return 0;
}