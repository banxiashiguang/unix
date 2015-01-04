#include "common.h"

int main(int argc, char const *argv[])
{
	int client_fd;
	struct sockaddr_in server_addr;

	if(argc != 2)
		cout<<"Usage:udpclient01 <IP Address>"<<endl;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	client_fd = Socket(AF_INET, SOCK_DGRAM, 0);

	dg_client(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//dg_check_client(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//dg_connect_client(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	//dg_fixed_client(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	close(client_fd);
	return 0;
}