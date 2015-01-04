#include "common.h"

int main(int argc, char const *argv[])
{
	int client_fd;
	struct sockaddr_in server_addr;

	if(argc != 2)
	{
		cout<<"Usage:tcpclient01 <IP address>"<<endl;
		return -1;
	}

	client_fd = Socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	Connect(client_fd, (struct sockaddr*)&server_addr,sizeof(server_addr));
	shutdown_str_cli(client_fd);
	exit(0);
}