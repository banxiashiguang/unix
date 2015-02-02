#include "web.h"

void home_page(const char *host,const char *fname)
{
	int fd,n;
	char line[MAXLEN];

	fd = tcp_connect(host, SERV);
	n = snprintf(line,sizeof(line),GET_CMD,fname);
	Writen(fd, line, n);
	while(1)
	{
		if((n = read(fd, line, MAXLEN)) == 0)
			break;
		cout<<"read from host "<<n<<" bytes"<<endl;
	}
	cout<<"返回内容："<<line<<endl;
	cout<<"read finished"<<endl;
	close(fd);
}

void start_connect(struct file *filename)
{
	int fd,flags,n;
	struct addrinfo *ai;
	ai = host_serv(filename->f_host, SERV, 0, SOCK_STREAM);//完成从域名、服务名到IP、端口的转换
	fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	//设置非阻塞
	flags = fcntl(fd, F_GETFL,0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	filename->f_fd = fd;
	cout<<"start_connect for "<<filename->f_name<<" fd="<<fd<<endl;

	if((n = connect(fd, ai->ai_addr, ai->ai_addrlen)) < 0)
	{
		if(errno != EINPROGRESS)
		{
			cout<<"nonblock connect errro"<<endl;
			return;
		}
		filename->f_flags = F_CONNECTING;
		FD_SET(fd, &rset);
		FD_SET(fd, &wset);
		if(fd > maxfd)
			maxfd = fd;
	}else{//connect 建立成功
		write_get_cmd(filename);
	}
}

void write_get_cmd(struct file *filename)
{
	int n;
	char line[MAXLEN];

	n = snprintf(line,sizeof(line),GET_CMD,filename->f_name);
	cout<<"send command："<<line<<endl;
	Writen(filename->f_fd, line, n);
	cout<<"write "<<n<<"bytes for "<<filename->f_name<<endl;
	filename->f_flags = F_READING;
	FD_SET(filename->f_fd, &rset);
	if(filename->f_fd > maxfd)
		maxfd = filename->f_fd;
}

int main(int argc, const char *argv[])
{
	int maxnconn,i,fd,error,n;
	FILE *fp;
	socklen_t len;
	char buff[MAXLEN];
	fd_set rs,ws;

	if(argc < 5)
	{
		cout<<"Usage:web <nconn> <hostname> <hostpage> <filename1> ……"<<endl;
		return -1;
	}

	home_page(argv[2], argv[3]);

	maxnconn = atoi(argv[1]);
	nfiles = min(argc-4,MAXFILES);
	for(int i=0;i<nfiles;i++)
	{
		file[i].f_name = (char*)argv[4+i];
		file[i].f_host = (char*)argv[2];
		file[i].f_flags = 0;
	}

	if((fp = fopen("image1.gif","ab")) == NULL)
	{
		perror("fopen error");
		return -1;
	}

	FD_ZERO(&rset);
	FD_ZERO(&wset);
	maxfd = -1;
	nlefttoconn = nlefttoread = nfiles;//nlefttoconn:待连接的数目
	nconn = 0;//已经连接的数目

	while(nlefttoread)
	{
		while(nconn < maxnconn && nlefttoconn)
		{
			for(i=0;i<nfiles;i++)
				if(file[i].f_flags == 0)//找到未处理的请求
					break;
			if(i == nfiles)
			{
				cout<<"files finished"<<endl;
				return 0;
			}
			start_connect(&file[i]);
			nconn++;
			nlefttoconn--;
		}

		rs = rset;
		ws = wset;
		select(maxfd+1, &rs, &ws, NULL, NULL);
		for(i=0;i<nfiles;i++)
		{
			if(file[i].f_flags == 0 || file[i].f_flags == F_DONE)
				continue;
			fd = file[i].f_fd;
			if(file[i].f_flags == F_CONNECTING && (FD_ISSET(fd, &rs) || FD_ISSET(fd, &ws)))
			{
				if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0)//connect failed
					cout<<"connect for "<<file[i].f_name<<" failed"<<endl;
				else
				{
					cout<<"connect successful for "<<file[i].f_name<<endl;
					FD_CLR(fd, &rs);
					write_get_cmd(&file[i]);
				}
			}else if(file[i].f_flags == F_READING && FD_ISSET(fd, &rs))
			{
				if((n = read(fd, buff, sizeof(buff))) == 0)//文件EOF
				{
					cout<<"end-of-file on "<<file[i].f_name<<endl;
					close(fd);
					file[i].f_flags = F_DONE;
					FD_CLR(fd, &rs);
					nconn --;
					nlefttoread --;
				}else{
					cout<<"read "<<n<<" bytes from "<<file[i].f_name<<endl;
					fwrite(buff,1,n,fp);
				}
			}
		}
	}
	fclose(fp);
	return 0;
}