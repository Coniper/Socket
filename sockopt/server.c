/*******************************************************
  > File Name: server.c
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
#define ERR_LOG(LOG,connfd)    do{perror(LOG); close(connfd); break;}while(0)   

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

    //设置套接字属性 应用层的地址和端口重用
    int on = 1;     //定义操作的方式 1(打开) 0(关闭)
    if(-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
    {
        ERR_LOG("setsockopt", sockfd);
    }

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
            bzero(buf, sizeof(buf));    //memset亦可

            //5、数据交互
#if 0
            if(-1 == (ret = read(connfd, buf, sizeof(buf))))
            {
                ERR_LOG("read", connfd);
            }
            else if(ret == 0)
            {
                printf("client is leave\n");
                close(connfd);
                break;
            }
            else if(NULL != strstr(buf, "quit"))    //strstr:字符串查找 //strcmp:字符串比较
            {
                printf("client request to leave\n");
                close(connfd);
                break;
            }
            else
            {
                printf("%s\n", buf);
            }

            printf("input: ");
            fgets(buf, sizeof(buf), stdin); 
            //fgets从stdin获取数据后会在末尾置'\0'，write发送时，使用的是strlen，估计垃圾数据不会发送

            if(-1 == write(connfd, buf, strlen(buf)))
            {
                ERR_LOG("write", connfd);
            }
#endif
            ret = recv(connfd, buf, sizeof(buf), 0);    //最后一个参数 0表示网络无数据就阻塞
            if(-1 == ret)
            {
                perror("recv");
                return -1;
            }
            else if(0 == ret)
            {
                printf("client already leaving\n");
                break;
            } 
            else 
            {
                printf("recv: %s\n", buf);
                if (strstr(buf, "quit") != NULL)       //字符串的查找函数，没有找到返回 NULL
                    break;
            }

            printf("input: ");
            fgets(buf, sizeof(buf), stdin);
            send(connfd, buf, strlen(buf), 0);
            printf("send success\n");
        }
        close(connfd);
    }

    //6、关闭套接字
    close(sockfd);

ERR_STOP:
    return 0;
}

