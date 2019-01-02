
#ifndef __SERVER_H__
#define __SERVER_H__

#include<stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
typedef struct serverinfo{
	int client_socket_fd;
	struct uloop_fd client_accfd;
}SERVER_INFO;


#endif
