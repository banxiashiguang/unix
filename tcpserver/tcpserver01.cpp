#include "common.h"

int main(int argc, char const *argv[])
{
	int listen_fd,connect_fd;
	socklen_t client_len;
	pid_t child_pid;
	struct sockaddr_in server_addr,client_addr;

	listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	Bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	Listen(listen_fd, QENLEN);

	if(signal(SIGCHLD, sig_chld) == SIG_ERR)//防止子程序结束之后变成将死程序。
		perror("signal error");
	cout<<"after signal"<<endl;

	while(1)
	{
		client_len = sizeof(client_addr);
		connect_fd = Accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
		if((child_pid = fork()) == 0)
		{
			close(listen_fd);
			str_echo(connect_fd);
			exit(0);//用来终结子程序的运行
		}
		close(connect_fd);
	}
	return 0;
}