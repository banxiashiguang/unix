#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
	union{
		short s;
		char c[sizeof(short)];
	}un;
	un.s = 0x0102;
	cout<<CPU_VENDOR_OS;
	if(sizeof(short) == 2)
	{
		if(un.c[0] == 2 && un.c[1] == 1)
			cout<<"little endian"<<endl;
		if(un.c[0] == 1 && un.c[1] == 2)
			cout<<"big endian"<<endl;
	}else{
		cout<<"short size:"<<sizeof(short)<<endl;
	}
	return 0;
}