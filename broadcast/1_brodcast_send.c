/*******************************************************
	> File Name: 1_broadcast_send.c
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
#define ERR_LOG(LOG) do{perror(LOG);break;}while(0)

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
    seraddr.sin_addr.s_addr = inet_addr("192.168.164.255");

    printf("udp client init success\n");

    char buf[BUFSZ] = "";
    
    //设置套接字允许广播
    int on = 1;
    if(-1 == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)))
    {
        ERR_LOG("setsockopt");
    }


    //交互
    while(1)
    {
        printf("input: ");
        fgets(buf, sizeof(buf), stdin);

        if(-1 == sendto(sockfd, buf, strlen(buf), 0, \
                    (struct sockaddr *)&seraddr, sizeof(seraddr)))
        {
            ERR_LOG("sendto");
        }
    }
    close(sockfd);

    return 0;
}

