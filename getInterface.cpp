#include "common.h"

int getInterfaceInfo(const char *name)
{
	char *ip = new char[16];
	char *broadcast = new char[16];
	char *netmask = new char[16];
	char *mac = new char[32];
	int sockfd;
	struct ifreq ifr;
	struct sockaddr_in *sin;

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket error");
		return -1;
	}
	memset(&ifr,0,sizeof(ifr));
	strcpy(ifr.ifr_name,name);
	memset(sin,0,sizeof(sin));

	//获取IP地址
	if(ioctl(sockfd,SIOCGIFADDR,&ifr) != -1)
	{
		sin = (struct sockaddr_in*)&ifr.ifr_addr;
		strcpy(ip,inet_ntoa(sin->sin_addr));
		cout<<"IP:"<<ip<<endl;
	}
	//获得广播地址
	if(ioctl(sockfd, SIOCGIFBRDADDR,&ifr) != -1)
	{
		sin = (struct sockaddr_in*)&ifr.ifr_broadaddr;
		strcpy(broadcast,inet_ntoa(sin->sin_addr));
		cout<<"BROADCAST:"<<broadcast<<endl;
	}

	//获得子网掩码
	if(ioctl(sockfd, SIOCGIFNETMASK, &ifr) != -1){
		sin = (struct sockaddr_in *)&ifr.ifr_netmask;
		strcpy(netmask,inet_ntoa(sin->sin_addr));
		cout<<"MASK:"<<netmask<<endl;
	}

	//获取mac地址
	if(ioctl(sockfd, SIOCGIFHWADDR,&ifr) != -1)
	{
		sin = (struct sockaddr_in*)&ifr.ifr_hwaddr;
		sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x"
			,(unsigned char)ifr.ifr_hwaddr.sa_data[0]
			,(unsigned char)ifr.ifr_hwaddr.sa_data[1]
			,(unsigned char)ifr.ifr_hwaddr.sa_data[2]
			,(unsigned char)ifr.ifr_hwaddr.sa_data[3]
			,(unsigned char)ifr.ifr_hwaddr.sa_data[4]
			,(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
		cout<<"mac:"<<mac<<endl;
	}


	//获得接口标志
	if(ioctl(sockfd, SIOCGIFFLAGS,&ifr) != -1)
	{
		if(ifr.ifr_flags & IFF_UP)
			cout<<"UP"<<endl;
		else
			cout<<"DOWN"<<endl;
	}

	//获得MTU的大小
	if(ioctl(sockfd, SIOCGIFMTU,&ifr) != -1)
	{
		cout<<"MTU="<<ifr.ifr_mtu<<endl;
	}
	
	delete [] ip;
	delete [] broadcast;
	delete [] netmask;
	delete [] mac;
	return 0;
}

int main(int argc, char const *argv[])
{
	getInterfaceInfo("eth0");
	return 0;
}