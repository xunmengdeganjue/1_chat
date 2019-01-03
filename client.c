
#include "client.h"

CLIENT_INFO *client_info;
static void show_message( const char *msg){

	printf("Message from Server:\n\033[32m%s\033[0m\n",msg);

}

static void send_message( const char *msg){
	int msg_len = 0;
	
	if(msg==NULL || msg[0] == '\n'){
		trace_err("The message is Null!\n");
		return ;
	}
	msg_len = send(client_info->socket_fd,msg,strlen(msg),0);//发送信息
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
}
static void uloop_ufd_add(int socket_fd){
	client_info->ulfd.fd = socket_fd;
	client_info->ulfd.cb = message_recv;
	client_info->ulfd.registered = false;
	client_info->ulfd.flags = ULOOP_READ;
	uloop_fd_add(&client_info->ulfd, ULOOP_READ);

}
static int connect_init(){
	struct sockaddr_in addr;
	int socket_fd = -1;
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
	client_info->socket_fd = connect_init();
	uloop_ufd_add(client_info->socket_fd);

}
static void cleanup_env(){
	
	close(client_info->socket_fd);
	

}
void for_input (  )
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
	client_init();
	signal(SIGIO,for_input);
	enable_kdb_signals();//设置输入时发送信号，设置输入为O_ASYNC
	uloop_run();

	uloop_done();
	
	cleanup_env();

	return 0;
}
