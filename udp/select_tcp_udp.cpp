#include "common.h"

int main(int argc, char const *argv[])
{
	int tcpfd,udpfd,connectfd,maxfd;
	struct sockaddr_in serveraddr,clientaddr;
	fd_set readset,testset;
	pid_t child_pid;
	char msg[MAXLEN];
	int nread;
	socklen_t len;
	const int on = 1;

	//tcp设置
	tcpfd = Socket(AF_INET, SOCK_STREAM, 0);
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	setsockopt(tcpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));//端口可以重复绑定
	Bind(tcpfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	Listen(tcpfd, QENLEN);

	//udp设置
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	Bind(udpfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	FD_ZERO(&readset);
	FD_SET(tcpfd, &readset);
	FD_SET(udpfd, &readset);

	maxfd = max(tcpfd, udpfd);

	signal(SIGCHLD, sig_chld);//处理僵死的子进程

	while(1)
	{
		testset = readset;
		if(select(maxfd+1, &testset, NULL, NULL, NULL) < 0)
		{
			if(errno = EINTR)//fork子程序完成后可能产生中断，select无法自动回复
				continue;
			else
			{
				perror("select error");
				return -1;
			}
		}
		if(FD_ISSET(tcpfd, &testset))
		{
			len = sizeof(clientaddr);
			connectfd = Accept(tcpfd, (struct sockaddr *)&clientaddr, &len);
			if((child_pid = fork()) == 0)//子程序执行过程
			{
				close(tcpfd);
				str_echo(connectfd);
				exit(0);//
			}
			close(connectfd);
		}
		if(FD_ISSET(udpfd, &testset))
		{
			len = sizeof(clientaddr);
			nread = recvfrom(udpfd, msg, MAXLEN, 0, (struct sockaddr *)&clientaddr, &len);
			sendto(udpfd, msg, nread, 0, (const struct sockaddr *)&clientaddr, len);
		}
	}
	close(tcpfd);
	close(udpfd);
	return 0;
}