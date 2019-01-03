#ifndef __SERVER_H__
#define __SERVER_H__

#include "common.h"

#include <sys/types.h>

typedef struct serverinfo 
{
	int socket_fd;
	struct uloop_fd client_accfd;
	struct uloop_timeout input_tm;

}SERVER_INFO;


#endif

