#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "common.h"
typedef struct clientinfo{
	int socket_fd;
	struct uloop_fd ulfd;
}CLIENT_INFO;

#endif
