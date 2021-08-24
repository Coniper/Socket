/*******************************************************
  > File Name: 1_fork_server.c
  > Author:Coniper
  > Describe: 
  > Created Time: 2021年08月20日
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <unistd.h>

#define BFSZ 128
#define ERR_LOG(LOG)    do{perror(LOG);exit(-1);}while(0)

int server_init(int port);
int wait_client(int listenfd);
void process(int connfd);

int main(int argc, char *argv[])
{
    if(2 != argc)
    {
        printf("Uage <port>\n");
        goto ERR_STOP;
    }

    int listenfd = server_init(atoi(argv[1]));      //调用初始化服务器函数

    char request[BFSZ] = "";

    for(;;)
    {
        int connfd = wait_client(listenfd);         //阻塞等待客户端连接
        if(-1 == connfd)
        {
            close(listenfd);
            return -1;
        }

        pid_t pid;
        if(0 == (pid = fork()))    //为连接成功的客户端创建紫禁城
        {
            process(connfd);    //调用函数，在函数中与客户端进行数据教化
            //...               //此处也可以直接与客户端进行数据交互
        }
        else if(0 > pid)
        {
            perror("fork");
            continue;
        }
    }

    close(listenfd);

ERR_STOP:
    return 0;
}

int server_init(int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd){
        ERR_LOG("socket");
    }

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    if(-1 == bind(sockfd, (const struct sockaddr *)&addr, (socklen_t)sizeof(addr)))
    {
        close(sockfd);
        ERR_LOG("bind");
    }

    if(-1 == listen(sockfd, 5))
    {
        close(sockfd);
        ERR_LOG("listen");
    }

    printf("server init success\n");
    
    printf("wait for a client\n");
    
    return sockfd;
}

int wait_client(int listenfd)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    int connfd = accept(listenfd, (struct sockaddr *)&addr, &addrlen);
    if(-1 == connfd)
    {
        ERR_LOG("accept");
    }
    printf("Client_ip: %s port: %u\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    return connfd;
}

void process(int connfd)
{
    char buf[BFSZ] = {0};
    while(1)
    {
        int ret = recv(connfd, buf, sizeof(buf), 0);
        if(-1 == ret)
        {
            perror("recv");
            break;
        }
        else if(0 == ret)
        {
            printf("client is leaving\n");
            break;
        }
        printf("recv: %s\n", buf);

        if(strstr(buf, "quit")) //判断数据是哪个客户端所发送，决定作什么功能
        {
            break;
        }
        else if( strstr(buf, ".jpg"))
        {
            printf("picture\n");
            //调用接口发送图片
        }
    }
    close(connfd);
}
