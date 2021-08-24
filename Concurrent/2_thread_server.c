/*******************************************************
  > File Name: 2_thread_server.c
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

#include <pthread.h>

#define BFSZ 128
#define ERR_LOG(LOG)    do{perror(LOG);exit(-1);}while(0)

int server_init(int port);
int wait_client(int listenfd);
void *process(void *arg);

int main(int argc, char *argv[])
{
    if(2 != argc)
    {
        printf("Uage <port>\n");
        goto ERR_STOP;
    }

    int listenfd = server_init(atoi(argv[1]));      //调用初始化服务器函数

    pthread_t tid;
    for(;;)
    {
        int connfd = wait_client(listenfd);         //阻塞等待客户端连接
        if(-1 == connfd)
        {
            close(listenfd);
            return -1;
        }
        
        //为来的客户端开启一个线程，在线程中与之进行数据交互
        if(-1 == pthread_create(&tid, NULL, process, (void *)&connfd))
        {
            printf("thread create failed\n");
            close(connfd);
            continue;
        }
        //分离线程，回收资源，使用pthread_join会阻塞，不能实现并发
        pthread_detach(tid);
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

void *process(void *arg)
{
    int connfd = *(int *)arg;   //先将void *arg 转成 int *， 代表该地址上存储的数据为 int 类型, 再取 * 表示取地址上的 前4字节

    char buf[BFSZ] = {0};
    while(1)
    {
        int ret = recv(connfd, buf, sizeof(buf), 0);
        if(-1 == ret)
        {
            perror("recv");
            return (void *)-1;
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
        else if(strstr(buf, ".jpg"))
        {
            printf("picture\n");
            //调用接口发送图片
        }
    }
    close(connfd);

    return (void *)0;
}
