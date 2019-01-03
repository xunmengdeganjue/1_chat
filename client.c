/*
	chat for socket coding test.
*/

#include "client.h"

int socket_fd ;
struct uloop_fd ul_fd;

CLIENT_INFO *client_info;

static void show_message( const char *msg){

	printf("Server:%s\n",msg);

}

static void send_message( const char *msg){
	int msg_len = 0;
	
	if(msg==NULL){
		trace_err("The message is Null!\n");
	}
	msg_len = send(socket_fd,msg,strlen(msg),0);//发送信息
	if(msg_len < 0){
		trace_err("Message send failed!\n");
	}
}

int message_recv(struct uloop_fd *u, unsigned int events){
	char buf[MSG_SIZE] = {0};
	if( events & ULOOP_READ){
		if(recv( u->fd, buf, sizeof(buf), 0) > 0){
			show_message(buf);
		}else{
			trace_err("receive error!\n");
		}
	}
	return 0;
}
static void uloop_ufd_add( int socket_fd){
	client_info->ufd.fd = socket_fd;
	client_info->ufd.cb = message_recv;
	client_info->ufd.registered = false;
	client_info->ufd.flags = ULOOP_READ;
	uloop_fd_add(&client_info->ufd, ULOOP_READ);

}
static int client_connect_init(){
	struct sockaddr_in addr;
	int socket_fd;
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
	return socket_fd;
	

}

static void client_init(){
	client_info = (CLIENT_INFO *)malloc(sizeof(CLIENT_INFO));
	client_info->socket_fd = client_connect_init();
	uloop_ufd_add(client_info->socket_fd);
	
}
static void cleanup_env(){
	
	close(socket_fd);
	

}
void client_for_input (  )
{
	char message[128] = {0};
	fgets(message,sizeof(message),stdin);
	if(message[0] != 0){
		printf("You have input the content:%s\n",message);
		send_message(message);
	}
}

//设置输入时发送信号，设置输入为O_ASYNC
void enable_kdb_signals()
{
	int fd_flags;
	fcntl(0,F_SETOWN,getpid());
	fd_flags = fcntl(0,F_GETFL);
	fcntl(0,F_SETFL,(fd_flags|O_ASYNC));
}


int main(int argc ,char **argv)
{

	uloop_init();
	
	signal(SIGIO, client_for_input);
	enable_kdb_signals();//设置输入时发送信号，设置输入为O_ASYNC

	client_init();
	
	uloop_run();

	uloop_done();
	cleanup_env();

	return 0;
}
