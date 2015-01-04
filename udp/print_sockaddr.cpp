#include "common.h"

int main(int argc, char const *argv[])
{
	int client_fd;
	struct sockaddr_in server_addr,localaddr;

	if(argc != 2)
		cout<<"Usage:udpclient01 <IP Address>"<<endl;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	client_fd = Socket(AF_INET, SOCK_DGRAM, 0);

	Connect(client_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr));

	memset(&localaddr,0,sizeof(localaddr));
	socklen_t len = sizeof(localaddr);
	Getsockname(client_fd, (struct sockaddr *)&localaddr, &len);

	char local[20];
	inet_ntop(AF_INET, &localaddr.sin_addr, local, 20);
	cout<<local<<endl;
}