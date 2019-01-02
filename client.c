
#include <stdio.h>
#include <string.h>

#include <libubox/uloop.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "logopt.h"


int socket_fd ;
struct uloop_fd ul_fd;

int message_recv(struct uloop_fd *u, unsigned int events){
	char buf[MSG_SIZE] = {0};
	if( events & ULOOP_READ){
		if(recv( u->fd, buf, sizeof(buf), 0) > 0){
			trace_info("recv_buf:%s\n", buf);
			int send_len = 0;
			fgets(buf,sizeof(buf),stdin);/*这种方法获取带有空格的字串比较可靠*/
			trace_info("get the message from the table!\n");
			send_len = send(u->fd,buf,sizeof(buf),0); 
		}else{
			trace_err("receive error!\n");
		}
	}
}
static void uloop_fd_init(){
	ul_fd.fd = socket_fd;
	ul_fd.cb = message_recv;
	ul_fd.registered = false;
	ul_fd.flags = ULOOP_READ;
	uloop_fd_add(&ul_fd, ULOOP_READ);

}
static int server_init(){
	struct sockaddr_in addr;
	socket_fd = socket(AF_INET,SOCK_STREAM,0);
	if(socket_fd < 0){
		printf("create the socket failed!\n");
	} 
	/*配置socket*/
	bzero(&addr, sizeof(addr)); 
	addr.sin_family = AF_INET;  /*设置成IP通信*/
	addr.sin_port = htons(CON_PORT);  /*设置服务器端口号*/
	addr.sin_addr.s_addr = INADDR_ANY;  /*设置服务器IP，INADDR_ANY表示使用本地IP地址，系统会自动填充本地IP地址；
	  也可以这样设置目标服务器IP地址 addr.sin_addr.s_addr = inet_addr("127.0.0.1");*/

	/*将客户端套接字绑定到服务器IP上去*/
	if(connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)  
	{  
		printf("bind error\n");  
		return -1;  
	} 
	
}
static void cleanup_env(){
	
	close(socket_fd);
	

}
int main(int argc ,char **argv)
{

	uloop_init();
	server_init();
	uloop_fd_init();
	
	uloop_run();

	uloop_done();
	
	cleanup_env();

	return 0;
}
