/*

*/

#include "server.h"

SERVER_INFO *server_info;

static void message_deal(struct uloop_fd *u, unsigned int events){
	char buf[1024] = {0};
	if( events & ULOOP_READ){
		if( recv( u->fd, buf, sizeof(buf), 0) > 0){
			/*receive the message from the client successfully*/
			trace_info("\nClient Say:%s\n",buf);
		}
	}
}
static void uloop_add(int socket_fd){
	server_info->client_accfd.fd = socket_fd;
	server_info->client_accfd.cb = message_deal;	
	uloop_fd_add(&server_info->client_accfd, ULOOP_READ);
}

static int get_client_access_fd(){
	int server_socketfd;//服务器端套接字 
	int client_socketfd;//客户端套接字
	struct sockaddr_in server_addr;//服务器网络地址结构体 
	struct sockaddr_in client_add;//客户端网络地址结构体	
	int addr_len = sizeof(server_addr);
	int client_len = 0;
	int mesge_len = 0;
	/*创建服务器端套接字--IPv4协议，面向连接通信，UDP协议*/  
	server_socketfd = socket(PF_INET, SOCK_STREAM, 0);	
	if(server_socketfd < 0)  
	{  
	printf("socket error\n");  
	return -1;	
	}
	//socket配置
	bzero(&server_addr, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET;  
	server_addr.sin_port = htons(CONNECT_PORT);	
	server_addr.sin_addr.s_addr = INADDR_ANY;
	//将服务器端套接字绑定到服务器IP上
	if(bind(server_socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)  
	{  
	printf("bind error\n");  
	return -1;	
	}  
	/*监听服务器套接字,监听队列长度为5*/
	if(listen(server_socketfd, 5) < 0)	
	{  
	printf("listen error\n");  
	return -1;	
	}
	char message[MESSAGE_SIZE]={"hello how are you?"};
	int mesge_send_len = strlen(message);
	client_len = sizeof(struct sockaddr_in);
	/*等待客户端连接请求到达*/ 
	client_socketfd = accept(server_socketfd,(struct sockaddr *)&client_add,&client_len);
	int send_len=0;
	send_len=send(client_socketfd,message,mesge_send_len,0);//发送信息
	printf("send lenght is:%d\n",send_len); //if the send_len is equal to the mesge_send_len,means the sending is successfully.

	return client_socketfd;

}


static void base_info_init(){
	server_info->client_socket_fd = get_client_access_fd();
}

static void default_init(){
	server_info = (SERVER_INFO*)malloc(sizeof(SERVER_INFO));
	server_info->client_socket_fd = -1;

}
static void clean_up(){

	close(server_info->client_socket_fd);
	uloop_fd_delete(&server_info->client_accfd);
}

//设置输入时发送信号，设置输入为O_ASYNC
void enable_kdb_signals()
{
        int fd_flags;
        fcntl(0,F_SETOWN,getpid());
        fd_flags = fcntl(0,F_GETFL);
        fcntl(0,F_SETFL,(fd_flags|O_ASYNC));
}

static int send_message(const char*msg){
	int send_len = 0;
	send_len=send(server_info->client_socket_fd,msg,strlen(msg),0);//发送信息
	if(send_len < 0){
		trace_err("\nMessage[%s] send failed!\n",msg);
		return -1;
	}
	return send_len;
	
}

void gain_input(){
	char message[MESSAGE_SIZE] = {0};
	fgets(message, sizeof(message), stdin);
	if(message[0] != 0){
		send_message(message);
	}

}

int main(int argc ,char **argv)
{
	uloop_init();
	default_init();
	base_info_init();
	/*异步处理用户输入信息*/
	enable_kdb_signals();
	signal(SIGIO,gain_input);

	uloop_run();

	uloop_done();

	return 0;

}



