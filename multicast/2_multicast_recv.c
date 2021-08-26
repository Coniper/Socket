/*******************************************************
	> File Name: 2_multicast_recv.c
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
    
    //建立组播，填写组播ip
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("238.0.0.1"); //组播
    mreq.imr_interface.s_addr = inet_addr("0.0.0.0");   //本机

    //设置套接字属性，将其加入多播组
    if(-1 == setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)))
    {
        ERR_LOG("setsockopt");
    }

    //填写三元组信息
    struct sockaddr_in seraddr;
    seraddr.sin_family      = AF_INET;
    seraddr.sin_port        = htons(8888);
    seraddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    //设置套接字允许广播
    int on = 1;
    if(-1 == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)))
    {
        ERR_LOG("setsockopt");
    }

    //绑定
    if(-1 == bind(sockfd, (const struct sockaddr *)&seraddr, (socklen_t)sizeof(seraddr)))
    {
        ERR_LOG("bind");
    }

    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);

    char buf[BUFSZ] = "";

    printf("udp server init success\n");

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
    }
    close(sockfd);

    return 0;
}

