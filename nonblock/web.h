#include "common.h"

#define MAXFILES 20
#define SERV "80" //http 端口
#define	GET_CMD "GET %s HTTP/1.0\r\n\r\n"

#define F_CONNECTING 1
#define F_READING 2
#define F_DONE 4

int nconn, nfiles, nlefttoconn, nlefttoread, maxfd;
fd_set rset,wset;

struct file
{
	char *f_name;
	char *f_host;
	int f_fd;
	int f_flags;
}file[MAXFILES];

void home_page(const char* host,const char* fname);//获取主页
void start_connect(struct file *fname);//发起非阻塞connect
void write_get_cmd(struct file *filename);//发送一个httpGET命令到服务器