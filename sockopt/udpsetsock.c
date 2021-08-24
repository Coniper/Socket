/*******************************************************
	> File Name: udpserver.c
	> Author:Coniper
	> Describe: 
	> Created Time: 2021年08月20日
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define BUFSZ 128
#define ERR_LOG(LOG) do{perror(LOG);exit(-1);}while(0)

int main(int argc, char *argv[])
{
    //创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == sockfd)
    {
        ERR_LOG("socket");
    }

    //填写三元组信息
    struct sockaddr_in seraddr;
    seraddr.sin_family      = AF_INET;
    seraddr.sin_port        = htons(8888);
    seraddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    //绑定
    if(-1 == bind(sockfd, (const struct sockaddr *)&seraddr, (socklen_t)sizeof(seraddr)))
    {
        ERR_LOG("bind");
    }

    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    char buf[BUFSZ] = "";

    printf("udp server init success\n");
    
    struct timeval tm;
    tm.tv_sec = 2;
    tm.tv_usec = 0;
    //设在套接字接收超时
    //设置超时时间为 2 s,如果2s后网络中没有可读数据，系统会让 recvfrom立即返回 -1
    if(-1 == setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm)))
    {
        perror("setsockfd");
        return -1;
    }

    //交互
    while(1)
    {
        bzero(buf, sizeof(buf));
        bzero(&cliaddr, sizeof(cliaddr));

        //阻塞等待接收客户端数据，并保存客户端 ip 地址 和 端口号
        if(-1 == recvfrom(sockfd, buf, sizeof(buf), 0, \
                    (struct sockaddr *)&cliaddr, &clilen))
        {
            perror("recvfrom");
            break;
        }

        //打印客户端信息
        printf("client: ip: %s, port: %u\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    
        printf("recvfrom: %s\n", buf);

        //给客户端发送数据
        printf("input: ");
        fgets(buf, sizeof(buf), stdin);
        if(-1 == sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr)))
        {
            perror("sendto");
            break;
        }
        printf("send succeed\n");
    }
    close(sockfd);

    return 0;
}

