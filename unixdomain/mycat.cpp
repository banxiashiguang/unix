#include "common.h"

int main(int argc, char const *argv[])
{
	int fd,n;
	char buffer[MAXLEN];
	if(argc != 2)
	{
		cout<<"Usage:mycat <pathname>"<<endl;
		return -1;
	}
	if((fd = my_open(argv[1], O_RDONLY)) < 0)
		perror("can not open file");
	while((n = read(fd, buffer, MAXLEN)) > 0)
		write(STDOUT_FILENO, buffer, n);
	return 0;
}