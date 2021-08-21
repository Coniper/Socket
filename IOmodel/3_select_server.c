/*******************************************************
  > File Name: 3_select_server.c
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

    //建立文件描述符集合(表)
    fd_set ifd, bfd;
    FD_ZERO(&bfd);
    FD_SET(listenfd, &bfd);

    char buf[BFSZ] = "";
    int nfds = listenfd;

    for(;;)
    {
        ifd = bfd;
        if(0 >= select(nfds + 1, &ifd, NULL, NULL, NULL))
        {
            perror("select");
            break;
        }
        for(int i = 0; i < nfds + 1; i++)
        {
            if(FD_ISSET(i, &ifd))
            {
                if(i == listenfd)
                {
                    int connfd = wait_client(listenfd);         //循环调用客户端连接函数
                    FD_SET(connfd, &bfd);                       //将连接套接子加入表中，准备下次循环时检测，以便读取客户端
                    nfds = nfds > connfd ? nfds : connfd;
                }
                else
                {
                    bzero(buf, sizeof(buf));
                    int ret = read(i, buf, sizeof(buf));
                    if(0 > ret)
                    {
                        perror("read");
                        continue;
                    }
                    else if(0 == ret)
                    {
                        printf("client is leaving\n");
                        FD_CLR(i, &bfd);                    //剔除掉离开的客户端
                        nfds = nfds > i ? nfds : (i - 1);   //调整nfds集合大小 防止轮循空走
                        close(i);                           //关闭没用的文件爱你描述符
                        printf("remain %d client\n", nfds - 3);
                        break;
                    }
                    printf("recv: %s\n", buf);
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
