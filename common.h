#ifndef __COMMON_H__
#define __COMMON_H__

#include <unistd.h>

#define CON_PORT 6666
#define MSG_SIZE 1024

typedef struct serverinfo 
{
	int socket_fd;
	struct uloop_fd client_accfd;
	struct uloop_timeout input_tm;

}SERVER_INFO;

#endif