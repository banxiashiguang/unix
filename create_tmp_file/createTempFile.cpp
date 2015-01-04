#include <iostream>
#include <cstdio>
using namespace std;

int main(int argc, char const *argv[])
{
	char *name;
	if(argc != 3)
		cout<<"Usage:a.out <directory> <prefix>"<<endl;
	if((name = tempnam(argv[1],argv[2])) == NULL)
		perror("tempnam");
	cout<<"file path="<<name<<endl;
	return 0;
}
