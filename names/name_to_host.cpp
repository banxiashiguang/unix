#include "common.h"

int main(int argc, char const *argv[])
{
	struct hostent* hptr;
	char **aptr;
	char str[INET_ADDRSTRLEN];
	if(argc != 2)
		cout<<"Usage:name_to_host domain name"<<endl;
	if((hptr = gethostbyname(argv[1])) == NULL)
	{
		cout<<"gethostbyname error:"<<hstrerror(h_errno)<<endl;
		return -1;
	}

	//规范名字
	cout<<"h_name="<<hptr->h_name<<endl;
	//别名
	for(aptr = hptr->h_aliases;*aptr != NULL;aptr++)
		cout<<"h_aliases:"<<*aptr<<endl;

	//IP地址
	switch(hptr->h_addrtype)
	{
		case AF_INET:
			cout<<"IP List:"<<endl;
			for(aptr = hptr->h_addr_list;*aptr != NULL;aptr++)
			{
				inet_ntop(AF_INET, *aptr, str, sizeof(str));
				cout<<"IP："<<str<<endl;
			}
			break;
		default:
			cout<<"unknow address type";
			break;
	}
	return 0;
}