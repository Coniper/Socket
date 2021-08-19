/*******************************************************
  > File Name: 1_socket.c
  > Author:Coniper
  > Describe: 
  > Created Time: 2021年08月19日
 *******************************************************/
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <unistd.h>

#define BUFSZ 128

int main(int argc, char *argv[])
{
    //1.创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > sockfd)
    {
        perror("socket");
        goto ERR_STOP;
    }

    printf("sockfd: %d\n", sockfd);

    //2、绑定
    //填写IP地址、端口号
    struct sockaddr_in addr;                        //定义一个Internet协议地址结构体变量，用来存储ip地址、端口号
    addr.sin_family      = AF_INET;                 //填写IPv4协议
    addr.sin_port        = htons(8888);             //填写端口号，必须将主机字节序转换成网络字节序短整形
    //addr.sin_addr.s_addr = inet_addr("0.0.0.0");  //准备绑定系统任意一个IP          
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(0 > bind(sockfd, (const struct sockaddr *)&addr, (socklen_t)sizeof(addr)))
    {
        perror("bind");
        goto ERR_STOP;
    }

    //3、监听套接字
    if(0 > listen(sockfd, 5))   //listen会在内核开辟一个存储客户端连接请求的队列，同时设置该队列的最大长度
    {
        perror("listen");
        goto ERR_STOP;
    }
    printf("server init success\n");

    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    char buf[BUFSZ] = {0};

    while(1)
    {
        printf("wait for a client\n");
        
        //4、阻塞等待，接受客户端的接连(从缓冲队列中出队客户端请求，建立好连接)
        int connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
        if(0 > connfd)
        {
            perror("accept");
            goto ERR_STOP;
        }
        printf("connfd: %d ip: %s port: %u\n", connfd, inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        while(1)
        {
            int ret = 0;
            bzero(buf, sizeof(buf));
            
            //5、数据交互
            if(-1 == (ret = read(connfd, buf, sizeof(buf))))
            {
                perror("read");
                close(connfd);
                break;
            }
            else if(ret == 0)
            {
                printf("client is leave\n");
                close(connfd);
                break;
            }
            else if(NULL != strstr(buf, "quit"))
            {
                close(connfd);
                break;
            }
            
            printf("%s\n", buf);
        
            memset(buf, 0, sizeof(buf));
            printf("input: ");
            fgets(buf, sizeof(buf), stdin);
            
            if(-1 == write(connfd, buf, strlen(buf)))
            {
                perror("write");
                close(connfd);
                break;
            }
        }
    }

    //6、关闭套接字
    close(sockfd);

ERR_STOP:
    return 0;
}

