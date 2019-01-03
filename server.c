
/**

**/

#include "server.h"


SERVER_INFO *server_info;

static void show_message( const char *msg){

	printf("Message from Client:\n\033[32m%s\033[0m\n",msg);

}
static void client_message_deal(struct uloop_fd *u, unsigned int events){

	char buf[MSG_SIZE] = {0};

	if(events & ULOOP_READ){
		if(recv( u->fd, buf, sizeof(buf), 0) > 0){
			//trace_info("recv_buf:%s\n", buf);
			show_message(buf);
		}
	}
}
static void ul_fd_add(int socket_fd){
	server_info->client_accfd.cb = client_message_deal; 
	server_info->client_accfd.fd = socket_fd;
	server_info->client_accfd.registered = false;
	server_info->client_accfd.flags = ULOOP_READ;
	uloop_fd_add(&(server_info->client_accfd), ULOOP_READ);

}
static int connect_init(){

	int server_socketfd;//服务器端套接字 
	int client_socketfd;//客户端套接字
	struct sockaddr_in server_addr;//服务器网络地址结构体 
	struct sockaddr_in client_add;//客户端网络地址结构体	
	//int addr_len = sizeof(server_addr);
	int client_len = 0 ;
	//int mesge_len = 0;
	char message[1024]={"Welcom!"};
	
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
	server_addr.sin_port = htons(CON_PORT);  
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

	int mesge_send_len = strlen(message);
	client_len = sizeof(struct sockaddr_in);
	
	/*等待客户端连接请求到达*/ 
	client_socketfd = accept(server_socketfd, (struct sockaddr *)&client_add, (socklen_t *)&client_len);
	if(client_socketfd > 0){
		int send_len=0;
			send_len=send(client_socketfd,message,mesge_send_len,0);//发送信息
			printf("send lenght is:%d\n",send_len); //if the send_len is equal to the mesge_send_len,means the sending is successfully.

	}
	return client_socketfd;
	

}

static void server_init(){

	server_info = (SERVER_INFO *)malloc(sizeof(SERVER_INFO));
	server_info->socket_fd = connect_init();
	ul_fd_add(server_info->socket_fd);

}
static void send_message( const char *msg){
	int msg_len = 0;
	
	if(msg==NULL){
		trace_err("The message is Null!\n");
	}
	msg_len = send(server_info->socket_fd,msg,strlen(msg),0);//发送信息
	if(msg_len < 0){
		trace_err("Message send failed!\n");
	}
}
void for_input (  )
{
	char message[128] = {0};
	fgets(message,sizeof(message),stdin);
	if(message[0] != 0){
		printf("You have input the content:\n\033[32m%s\033[0m\n",message);
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

void clean_up(){
	close(server_info->socket_fd);
}
int main(int argc ,char **argv)
{
	uloop_init();
	
	/*为实现收发操作异步进行，添加以下两行代码*/
	signal(SIGIO,for_input);
	enable_kdb_signals();//设置输入时发送信号，设置输入为O_ASYNC
		
	server_init();
	
	uloop_run();
	uloop_done();
	clean_up();
	return 0;
 
}
