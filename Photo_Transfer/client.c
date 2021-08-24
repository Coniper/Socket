/*******************************************************
  > File Name: 2_tcp_client.c
  > Author:Coniper
  > Describe: 
  > Created Time: 2021年08月19日
 *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>

#define BUFSIZE 128

int main(int argc, char *argv[])
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd)
    {
        perror("socket");
        goto ERR_STOP;
    }

    struct sockaddr_in sockaddr;
    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_port        = htons(8888);
    sockaddr.sin_addr.s_addr = inet_addr("192.168.164.128");

    //绑定(可选)

    //3、建立连接
    while(1)
    {
        if(0 != connect(sockfd, (const struct sockaddr *)&sockaddr, (socklen_t)sizeof(sockaddr)))
        {
            perror("connect");
            goto ERR_STOP;
        }

        char buf[BUFSIZE] = "0";

        //4、交互
        while(1)
        {
            printf("input: ");
            fgets(buf, sizeof(buf), stdin);
            send(sockfd, buf, strlen(buf), 0);

            size_t ret = recv(sockfd, buf, sizeof(buf), 0);
            if(-1 == ret)
            {
                perror("recv");
                close(sockfd);
                break;
            }
            else if(0 == ret)
            {
                printf("server network anomaly\n");
                break;
            }

            if(NULL != strstr(buf, "error:"))
            {
                continue;
            }
            int fd = open("2.jpeg", O_WRONLY | O_CREAT, 0640);
            if(-1 == fd)
            {
                perror("open");
                break;
            }
            //将地址上的图片长度数据解析出来
            long piclen = atoi(buf);
            char buf[BUFSIZE] = {0};
            int count = 0;
            while(count < piclen)
            {
                int ret = recv(sockfd, buf, sizeof(buf), 0);
                if(0 >= ret)
                {
                    break;
                }
                write(fd, buf, ret);
                count += ret;
            }
            close(fd);
        }
    }

    close(sockfd);

ERR_STOP:
    return 0;
}

