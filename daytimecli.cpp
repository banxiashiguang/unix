#include "common.h"

int main(int argc, char const *argv[])
{
	int sockfd;
	struct sockaddr storage;
	socklen_t len = sizeof(storage);
	char recv[MAXLEN];

	if(argc != 3)
		cout<<"Usage:daytimecli <hostname/IP address> <servicename/port>"<<endl;
	
	sockfd = tcp_connect(argv[1], argv[2]);

	getpeername(sockfd, &storage, &len);

	memset(recv,0,sizeof(recv));
	while(read(sockfd, recv, MAXLEN) > 0)
		cout<<recv;
	return 0;
}