/*******************************************************
  > File Name: server.c
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
        int connfd = wait_client(listenfd);
        if(-1 == connfd)
        {
            close(listenfd);
            return -1;
        }
        while(1)
        {
            bzero(request, sizeof(request));
            int ret = recv(connfd, request, sizeof(request), 0);
            if(0 > ret)
            {
                perror("recv");
                break;
            }
            else if(0 == ret)
            {
                printf("client is leaving\n");
                break;
            }
            else
            {
                request[strlen(request)-1] = '\0';
                if(NULL != strstr(request, ".jpeg"))
                {
                    int fd = open(request, O_RDONLY);
                    if(0 > fd)
                    {
                        send(connfd, "error: open jpeg failed"/*stderror()*/, 20, 0);
                        continue;
                    }
                    long len = lseek(fd, 0, SEEK_END);
                    char *buf = (char *)malloc(len);
                    lseek(fd, 0, SEEK_SET);
                    printf("%ld\n", read(fd, buf, len));

                    //一、发送图片长度
                    //send(connfd, &len, 20, 0);
                    
                    char response[20] = "";
                    sprintf(response, "%ld", len);
                    printf("len: %ld\n", send(connfd, response, 20, 0));

                    //二、根据长度发送图片
                    printf("pic: %ld\n", send(connfd, buf, len, 0));
                }
                else
                {
                    recv(connfd, request, sizeof(request), 0);
                    printf("request: %s\n", request);
                    memset(request, 0, BFSZ);
                }
            }
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
