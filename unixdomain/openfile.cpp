#include "common.h"

int main(int argc, char const *argv[])
{
	int fd,n;
	char buffer[MAXLEN];

	if(argc != 4)
	{
		cout<<"Usage:openfile <sockfd> <filename> <mode>"<<endl;
		return -1;
	}
	if((fd = open(argv[2],atoi(argv[3]))) < 0)
		exit( (errno > 0)? errno:255);
	if(write_fd(atoi(argv[1]), (void*)"", 1, fd) < 0)
	{
		perror("write_fd error");
		exit((errno > 0)? errno:255);
	}
	return 0;
}