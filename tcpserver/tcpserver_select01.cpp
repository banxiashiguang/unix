#include "common.h"

int main(int argc, char const *argv[])
{
	int listen_fd,connect_fd;
	socklen_t client_len;
	struct sockaddr_in server_addr,client_addr;
	int client[FD_SETSIZE];//保存连接的socket套接字
	fd_set readset,testset;
	int nready,i;//select中准备好的socket数目
	int maxfd,maxi;//maxfd:socket最大值.
	char buf[MAXLEN];
	int nbytes;

	listen_fd = Socket(AF_INET, SOCK_STREAM, 0);

	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	Bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	Listen(listen_fd, QENLEN);

	maxfd = listen_fd;
	maxi = -1;

	for(i = 0;i < FD_SETSIZE;i++)
		client[i] = -1;
	FD_ZERO(&readset);
	FD_SET(listen_fd,&readset);

	while(1)
	{
		testset = readset;
		nready = select(maxfd+1,&testset,NULL,NULL,NULL);
		if(FD_ISSET(listen_fd, &testset))
		{
			client_len = sizeof(client_addr);
			connect_fd = Accept(listen_fd, (struct sockaddr *)&client_addr,&client_len);

			for(i = 0;i < FD_SETSIZE;i++)
			{
				if(client[i] == -1)
				{
					client[i] = connect_fd;
					cout<<"i = "<<i<<",client["<<i<<"] = "<<client[i]<<endl;
					break;
				}
			}

			if(i == FD_SETSIZE)
			{
				cout<<"too many client"<<endl;
				return -1;
			}

			FD_SET(connect_fd, &readset);
			if(connect_fd > maxfd)
				maxfd = connect_fd;
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}

		for(i = 0;i <= maxi;i++)
		{
			if((connect_fd = client[i]) < 0)
				continue;
			if(FD_ISSET(connect_fd, &testset))
			{
				memset(buf,0,MAXLEN);
				if((nbytes = read(connect_fd, buf, MAXLEN)) == 0)//connection close by client
				{
					cout<<"connection close by client"<<endl;
					close(connect_fd);
					FD_CLR(connect_fd, &readset);
					client[i] = -1;
				}else{
					cout<<buf<<endl;
					Writen(connect_fd, buf, nbytes);
				}
				if(--nready < 0)
					break;
			}
		}
	}
	return 0;
}