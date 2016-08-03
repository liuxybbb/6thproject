#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "head.h"


int main(int argc, const char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: app <led> \n");
		return 0;
	}
	int fd;

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{

		perror("fail to open \n");
		return -1;
	}

	while (1) {
		sleep(1);
		ioctl(fd, A);
		sleep(1);
		ioctl(fd, B);
	}
	return 0;
}
