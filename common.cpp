#include "common.h"

int Socket(int family,int type,int protocol)
{
	int socket_fd;
	if((socket_fd = socket(family,type,protocol)) < 0)
	{
		perror("socket error");
		return 0;
	}
	return socket_fd;
}

void Bind(int fd,const struct sockaddr* addr,socklen_t len)
{
	if(bind(fd,addr,len) < 0)
		perror("bind error");
}

void Listen(int fd,int qlen)
{
	if(listen(fd,qlen) < 0)
		perror("listen error");
}

int Accept(int fd,struct sockaddr* addr,socklen_t* len)
{
	int client_fd;
	client_fd = accept(fd, addr, len);
	if(client_fd < 0)
	{
		perror("accept error");
		return -1;
	}
	return client_fd;
}
ssize_t writen(int fd,const void *buf,size_t n)
{
	size_t nleft;
	ssize_t nwrite;
	const char* p;

	nleft = n;
	p = (const char *)buf;
	while(nleft > 0)
	{
		if((nwrite = write(fd, p, nleft)) <= 0)
		{
			if(nwrite == 0 && errno == EINTR)//外部中断
				nwrite = 0;
			else
				return -1;	
		}
		nleft -= nwrite;
		p += nwrite;
	}
	return n;
} 

void Writen(int fd,void *buf,size_t nbytes)
{
	if(writen(fd, buf, nbytes) != nbytes)
		perror("writen error");
}

void str_echo(int fd)
{
	int n;
	char buf[MAXLEN];

again:
	while((n = read(fd, buf, MAXLEN)) > 0)
	{
		Writen(fd, buf, n);
	}
	if(n == 0 && errno == EINTR)
		goto again;
	else
		perror("str_echo error:read error");
}

void str_echo_plus(int fd)
{
	int n;
	int arg1,arg2;
	char buf[MAXLEN];

again:
	while((n = read(fd, buf, MAXLEN)) > 0)
	{
		if(sscanf(buf,"%d%d",&arg1,&arg2) == 2)
			snprintf(buf,sizeof(buf),"%d ",arg1+arg2);
		Writen(fd, buf, n);
	}
	if(n == 0 && errno == EINTR)
		goto again;
	else
		perror("str_echo error:read error");
}

void str_cli(int fd)
{
	string str;
	char readBuf[MAXLEN];
	while((getline(cin,str) > 0))
	{
		const char* p = str.c_str();
		char* copy = new char[strlen(p) + 1];
		strcpy(copy,p);
		Writen(fd, copy, strlen(copy));
		delete [] copy;

		memset(readBuf,0,sizeof(readBuf));
		read(fd, readBuf, MAXLEN);
		cout<<readBuf<<endl;
	}
}

//阻塞套接字
void select_str_cli(int fd)
{
	int max_fd;
	fd_set read_set,test_set;
	string str;
	char readBuf[MAXLEN];

	int input_fd = fileno(stdin);
	FD_ZERO(&read_set);
	FD_SET(fd,&read_set);
	FD_SET(input_fd, &read_set);

	max_fd = max(fd,input_fd);

	while(1)
	{
		test_set = read_set;
		if(select(max_fd+1,&test_set,NULL,NULL,NULL) < 0)
			perror("select error");
		if(FD_ISSET(fd, &test_set))
		{
			memset(readBuf,0,sizeof(readBuf));
			read(fd,readBuf,MAXLEN);
			cout<<readBuf<<endl;
		}
		if(FD_ISSET(input_fd, &test_set))
		{
			getline(cin,str);
			if(cin.eof())
			{
				cout<<"文件末尾"<<endl;
				return;
			}
			const char* p = str.c_str();
			char* copy = new char[strlen(p) + 1];
			strcpy(copy,p);
			Writen(fd, copy, strlen(copy));
			delete [] copy;
		}
	}
}

//非阻塞型套接字
void select_str_cli_noblock(int sockfd)
{
	int maxfd,val,flags,nread,nwrite;
	char to[MAXLEN],fr[MAXLEN];
	char *tooptr,*toiptr,*froptr,*friptr;
	fd_set readset,writeset;

	//非阻塞socket
	val = fcntl(sockfd, F_GETFL,0);
	fcntl(sockfd, F_SETFL,val | O_NONBLOCK);

	//非阻塞输入
	val = fcntl(STDIN_FILENO, F_GETFL,0);
	fcntl(STDIN_FILENO, F_SETFL, val | O_NONBLOCK);

	//非阻塞输出
	val = fcntl(STDOUT_FILENO, F_GETFL, 0);
	fcntl(STDOUT_FILENO, F_SETFL, val | O_NONBLOCK);

	tooptr = toiptr = to;
	froptr = friptr = fr;
	maxfd = max(max(STDIN_FILENO, STDOUT_FILENO), sockfd);
	flags = 0;//标识是否达到文件结尾

	while(1)
	{
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		if(tooptr != &to[MAXLEN] && flags == 0)
			FD_SET(STDIN_FILENO, &readset);
		if(tooptr != toiptr)
			FD_SET(sockfd, &writeset);
		if(friptr != &fr[MAXLEN])
			FD_SET(sockfd, &readset);
		if(froptr != friptr)
		{
			cout<<"FD_SET STDOUT_FILENO"<<endl;
			FD_SET(STDOUT_FILENO, &writeset);
		}
		select(maxfd+1, &readset, &writeset, NULL, NULL);

		if(FD_ISSET(STDIN_FILENO, &readset))
		{
			if((nread = read(STDIN_FILENO, toiptr, &to[MAXLEN]-toiptr)) < 0)
			{
				if(errno != EWOULDBLOCK)//读缓冲区有数据
					perror("read error");
			}else if(nread == 0){//到文件结尾
				cout<<"STDIN_FILENO 数据接受完成"<<endl;
				flags = 1;
				if(tooptr == toiptr)
					shutdown(sockfd, SHUT_WR);//关闭写端
			}else{
				toiptr += nread;
				FD_SET(STDOUT_FILENO, &writeset);
			}
		}

		if(FD_ISSET(sockfd, &readset))
		{
			if((nread = (read(sockfd, friptr, &fr[MAXLEN]-friptr))) < 0)
			{
				if(errno != EWOULDBLOCK)
					perror("read error");
			}else if(nread == 0){
				cout<<"socket 数据接受完成"<<endl;
				if(flags)
					return;
			}else{
				friptr += nread;
				FD_SET(STDOUT_FILENO, &writeset);
			}
		}

		if(FD_ISSET(STDOUT_FILENO, &writeset) && (friptr-froptr) > 0)
		{
			if((nwrite = write(STDOUT_FILENO, froptr, friptr-froptr)) < 0)
			{
				if(errno != EWOULDBLOCK)
					perror("write errror to STDOUT_FILENO");
			}else{
				froptr += nwrite;
				if(froptr == friptr)
				froptr = friptr = fr;
			}
		}

		if(FD_ISSET(sockfd, &writeset) && (toiptr-tooptr)>0)
		{
			if((nwrite = write(sockfd, tooptr, toiptr-tooptr)) < 0)
			{
				if(errno != EWOULDBLOCK)
					perror("write sockfd error");
			}else{
				tooptr += nwrite;
				if(tooptr == toiptr){
					tooptr = toiptr = to;
					if(flags)
						shutdown(sockfd, SHUT_RD);
				}
			}
		}
	}
}

//socket利用shutdown函数关闭写端
void shutdown_str_cli(int fd)
{
	int max_fd;
	fd_set read_set,test_set;
	string str;
	char readBuf[MAXLEN];

	int input_fd = fileno(stdin);
	FD_ZERO(&read_set);
	FD_SET(fd,&read_set);
	FD_SET(input_fd, &read_set);

	max_fd = max(fd,input_fd);

	while(1)
	{
		test_set = read_set;
		if(select(max_fd+1,&test_set,NULL,NULL,NULL) < 0)
			perror("select error");
		if(FD_ISSET(fd, &test_set))
		{
			memset(readBuf,0,sizeof(readBuf));
			if(read(fd,readBuf,MAXLEN) > 0)
				cout<<readBuf<<endl;
			else
			{
				cout<<"没有可读数据"<<endl;
				return;
			}
		}
		if(FD_ISSET(input_fd, &test_set))
		{
			getline(cin,str);
			if(cin.eof())
			{
				cout<<"文件末尾"<<endl;
				cout<<"关闭写端"<<endl;
				shutdown(fd,SHUT_WR);//关闭写端
				FD_CLR(fileno(stdin), &read_set);
				continue;
			}
			const char* p = str.c_str();
			char* copy = new char[strlen(p) + 1];
			strcpy(copy,p);
			Writen(fd, copy, strlen(copy));
			delete [] copy;
		}
	}
}

int max(int a1,int a2)
{
	if(a1 > a2)
		return a1;
	else
		return a2;
}

void Connect(int fd,const struct sockaddr * addr,socklen_t len)
{
	if(connect(fd, addr, len) < 0)
	{
		perror("connect error");
	}
}

//信号处理函数
void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		cout<<"child "<<pid<<" terminated"<<endl;
}

//UDP服务器回射函数
void dg_echo(int fd,struct sockaddr* client_addr,socklen_t client_len)
{
	int nread;
	char recvBuf[MAXLEN];
	while(1)
	{
		memset(recvBuf,0,sizeof(recvBuf));
		nread = recvfrom(fd, recvBuf, MAXLEN, 0, client_addr, &client_len);
		sendto(fd, recvBuf, nread, 0, client_addr, client_len);
	}
}

void dg_client(int fd,struct sockaddr* server_addr,socklen_t server_len)
{
	string str;
	char readBuf[MAXLEN];
	while((getline(cin,str) > 0))
	{
		const char* p = str.c_str();
		char* copy = new char[strlen(p) + 1];
		strcpy(copy,p);
		sendto(fd, copy, strlen(copy), 0, server_addr, server_len);
		delete [] copy;

		memset(readBuf,0,sizeof(readBuf));
		recvfrom(fd, readBuf, MAXLEN, 0, NULL, NULL);
		cout<<readBuf<<endl;
	}
}

void dg_check_client(int fd,struct sockaddr* server_addr,socklen_t server_len)
{
	string str;
	char readBuf[MAXLEN];
	socklen_t len;
	struct sockaddr* replay_addr = (struct sockaddr*)malloc(server_len);

	while((getline(cin,str) > 0))
	{
		const char* p = str.c_str();
		char* copy = new char[strlen(p) + 1];
		strcpy(copy,p);
		sendto(fd, copy, strlen(copy), 0, server_addr, server_len);
		len = server_len;
		delete [] copy;

		memset(readBuf,0,sizeof(readBuf));
		recvfrom(fd, readBuf, MAXLEN, 0, replay_addr, &len);
		if((len != server_len) || (memcmp(replay_addr,server_addr,len) != 0))
		{
			cout<<"不是服务器发送的数据"<<endl;
			continue;
		}
		cout<<readBuf<<endl;
	}
}

void dg_connect_client(int fd,struct sockaddr* server_addr,socklen_t server_len)
{
	string str;
	char readBuf[MAXLEN];
	struct sockaddr_in client_addr;
	int len;
	char IP[20];

	Connect(fd, server_addr, server_len);

	while((getline(cin,str) > 0))
	{
		const char* p = str.c_str();
		char* copy = new char[strlen(p) + 1];
		strcpy(copy,p);
		Writen(fd, copy, strlen(copy));
		delete [] copy;

		memset(readBuf,0,sizeof(readBuf));		
		if(read(fd, readBuf, MAXLEN) < 0)
		{
			perror("read error");
		}
		cout<<readBuf<<endl;
	}
}

void dg_fixed_client(int fd,struct sockaddr* server_addr,socklen_t server_len)
{
	int n;
	char sendBuf[DLEN];
	for(int i=0;i<NDG;i++)
	{
		n = sendto(fd, sendBuf, DLEN, 0, server_addr, server_len);
		cout<<n<<endl;
	}
}

void Getsockname(int fd,struct sockaddr* localaddr,socklen_t* local_len)
{
	if(getsockname(fd, localaddr, local_len) == 1)
		perror("getsockname");
}

struct addrinfo *host_serv(const char *hostname,const char *service,int family,int socktype)
{
	int return_value;
	struct addrinfo hints,*res;

	memset(&hints,0,sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;

	if((return_value = getaddrinfo(hostname, service, &hints, &res)) != 0)
		return NULL;
	return res;
}

int tcp_connect(const char *hostname,const char *service)
{
	int sockfd,n;
	struct addrinfo hints,*res,*ressave;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(hostname, service, &hints, &res);
	ressave = res;

	do{
		sockfd = socket(ressave->ai_family, ressave->ai_socktype, ressave->ai_protocol);
		if(sockfd < 0)
			continue;
		if(connect(sockfd, ressave->ai_addr, ressave->ai_addrlen) == 0)
			break;
		close(sockfd);
	}while((ressave = ressave->ai_next) != NULL);

	if(ressave == NULL)
	{
		cout<<"tcp connect error "<<hostname<<" "<<service<<endl;
	}

	freeaddrinfo(res);
	return sockfd;
}

static void connect_alarm(int signo)
{
	cout<<"connect_alarm"<<endl;
	return;
}

int connect_timeout(int sockfd,const struct sockaddr* serveraddr,socklen_t serverlen,int nsec)
{
	int n;
	signal(SIGALRM, connect_alarm);
	if(alarm(nsec) != 0)
		cout<<"alarm has already set"<<endl;
	if(connect(sockfd, serveraddr, serverlen) < 0)
	{
		close(sockfd);
		if(errno == EINTR)
			errno = ETIMEDOUT;
	}
	alarm(0);//关闭定时器
}

int write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
	struct msghdr	msg;
	struct iovec	iov[1];

//#ifdef	HAVE_MSGHDR_MSG_CONTROL
	union {
	  struct cmsghdr	cm;
	  char				control[CMSG_SPACE(sizeof(int))];
	} control_un;
	struct cmsghdr	*cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
/*#else
	msg.msg_accrights = (caddr_t) &sendfd;
	msg.msg_accrightslen = sizeof(int);*/
//#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return(sendmsg(fd, &msg, 0));
}

int read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
    struct msghdr msg;
    struct iovec iov[1];
    int n;
    int newfd;
//#ifdef HAVE_MSGHDR_MSG_CONTROL
    union{ // 对齐
	struct cmsghdr cm;
	char control[CMSG_SPACE(sizeof(int))];
    }control_un;
    struct cmsghdr *cmptr;
    // 设置辅助数据缓冲区和长度
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
/*#else
    msg.msg_accrights = (caddr_t) &newfd; // 这个简单
    msg.msg_accrightslen = sizeof(int);*/
//#endif 
    
    // TCP无视
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    // 设置数据缓冲区
    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    // 设置结束，准备接收
    if((n = recvmsg(fd, &msg, 0)) <= 0)
    {
        return n;
    }
//#ifdef HAVE_MSGHDR_MSG_CONTROL
    // 检查是否收到了辅助数据，以及长度，回忆上一节的CMSG宏
    cmptr = CMSG_FIRSTHDR(&msg);
    if((cmptr != NULL) && (cmptr->cmsg_len == CMSG_LEN(sizeof(int))))
    {
	// 还是必要的检查
        if(cmptr->cmsg_level != SOL_SOCKET)
        {
            printf("control level != SOL_SOCKET/n");
            exit(-1);
        }
        if(cmptr->cmsg_type != SCM_RIGHTS)
        {
            printf("control type != SCM_RIGHTS/n");
            exit(-1);
        }
	// 好了，描述符在这
        *recvfd = *((int*)CMSG_DATA(cmptr));
    }
    else
    {
        if(cmptr == NULL) printf("null cmptr, fd not passed./n");
        else printf("message len[%d] if incorrect./n", cmptr->cmsg_len);
        *recvfd = -1; // descriptor was not passed
    }
/*#else
    if(msg.msg_accrightslen == sizeof(int)) *recvfd = newfd; 
    else *recvfd = -1;*/
//#endif
    return n;
}

int my_open(const char* pathname,int mode)
{
	int fd,sockfd[2],status;
	pid_t childpid;
	char c,argsockfd[10],argmode[10];

	socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);//创建一个流管道

	if((childpid = fork()) == 0)//子程序
	{
		close(sockfd[0]);
		snprintf(argsockfd,10,"%d",sockfd[1]);//格式化字符串到char型数组中
		snprintf(argmode,10,"%d",mode);
		execl("./openfile", "openfile",argsockfd,pathname,argmode,(char*)NULL);
	}
	close(sockfd[1]);
	waitpid(childpid, &status, 0);
	if(WIFEXITED(status) == 0)
	{
		perror("unnormal exit");
	}
	if(WEXITSTATUS(status) == 0)
	{
		read_fd(sockfd[0],&c,1,&fd);
	}
	else
	{
		errno = status;
		fd = -1;
	}
	close(sockfd[0]);
	return fd;
}

//非阻塞的connect
int connect_nonblock(int fd,const sockaddr* serveraddr,socklen_t serverlen,int nsec)
{
	int val,resault,error;
	socklen_t len;
	fd_set rset,wset;
	timeval t;

	val = fcntl(fd, F_GETFL,0);
	fcntl(fd, F_SETFL,val | O_NONBLOCK);

	if((resault = connect(fd, serveraddr, serverlen)) < 0)
		if(errno != EINPROGRESS)
			return -1;
	if(resault == 0)//立马建立连接
	{
		cout<<"统一台主机"<<endl;
		goto done;
	}
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	wset = rset;
	t.tv_sec = nsec;
	t.tv_usec = 0;
	if(select(fd+1, &rset, &wset, NULL, nsec>0? &t:NULL) == 0)
	{
		close(fd);
		cout<<"TIMEOUT"<<endl;
		return -1;
	}
	if(FD_ISSET(fd, &rset) || FD_ISSET(fd, &wset))
	{
		len = sizeof(error);
		if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
			return -1;
	}

	done:
		fcntl(fd, F_SETFL,val);
		if(error)//连接建立错误
		{
			close(fd);
			errno = error;
			return -1;
		}
		return 0;
}	