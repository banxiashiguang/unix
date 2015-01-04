#include "common.h"

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
	dg_echo(server_fd, (struct sockaddr *)&client_addr, client_len);
	close(server_fd);
	return 0;
}