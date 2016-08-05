#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "chr_dev.h"
#include <sys/ioctl.h>

int main(int argc, const char *argv[])
{
	int  fd;
	char bufwrite[128]="user space";
	char bufread[128];

	fd = open("/dev/chr",0666);
	if(fd  <  0)
	{
	
		perror("open fail ");
		exit(EXIT_FAILURE);
	
	}
	printf("open  success   \n");

	printf("bufwrite:  %s\n",bufwrite);
	write(fd,bufwrite,128);

	read(fd,bufread,128);
	printf("bufread:  %s\n",bufread);
while(1)
{
	unsigned char tem = getchar();

	switch (tem)
	{
		
	case 'A':
		ioctl(fd,A);
		break;
	case 'B':
		ioctl(fd,B);
		break;
	case 'C':
		ioctl(fd,C);
		break;
	default:
		break;

	}
}

	close(fd);
	return 0;
}
