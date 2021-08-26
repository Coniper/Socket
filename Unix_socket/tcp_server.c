

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>

#include <sys/un.h>

#include <strings.h>
#include <string.h>

#define SPATH "/tmp/mysocket"

int main(int argc, char *argv[])
{
    //1、创建套接字
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ( 0 > sockfd){
        perror("socket");
        return -1;
    }
    printf("sockfd: %d\n", sockfd);

    //2、绑定
    // 1、填写本地协议、套接字文件
    struct sockaddr_un addr;        //定义一个本地协议地址结构体变量，用来存储协议和套接字文件
    addr.sun_family     = AF_UNIX;  //填写本地协议
    //检测套接字文件是否存在，如果存在就必须删除，如果不存在则在bind 时会自动创建
    if(0 == access(SPATH, F_OK)){
        unlink(SPATH);  //删除文件
    }
    strcpy(addr.sun_path, SPATH);   //填写套接字文件

    if ( 0 > bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))){
        perror("bind");
        return -1;
    }

    //3、监听套接字,在内核开辟一个存储客户端连接请求的队列，同时设置该队列的最大长度
    if ( 0 > listen(sockfd, 5)){
        perror("listen");
        return -1;
    }
    printf("server init success\n");
    while(1){ 
        printf("wait for a client!\n");
        //4、阻塞等待，接受客户端的连接（从缓冲队列中出队客户端的请求，建立好连接）
        
        int connfd = accept(sockfd, NULL, NULL);
        if ( 0 > connfd){
            perror("accept");
            return -1;
        }
      

        char buf[128] = {0};
        //5、数据交互
        while(1){
            bzero(buf, sizeof(buf));
            int ret = recv(connfd, buf, sizeof(buf), 0);    //最后一个参数 0表示网络无数据就阻塞
            if ( -1 == ret){
                perror("recv");
                return -1;
            } else if ( 0 == ret){
                printf("client already leaving\n");
                break;
            } else {
                printf("recv: %s\n", buf);
                if ( strstr(buf, "quit") != NULL){   //字符串的查找函数，没有找到返回 NULL
                    break;
                }
            }
            fgets(buf, sizeof(buf), stdin);
            send(connfd, buf, strlen(buf), 0);
        }
        close(connfd);
    }
    //6、关闭套接字
    close(sockfd);

    //作业：完善基于TCP的服务器程序，实现：
    //  1、服务器与模拟客户端的数据交互，先服务器接受客户端的数据并打印，
    //  2、服务器接收到 客户端数据之后，键盘输入数据回复客户端。
    //  3、当服务器接收到客户端的数据为 "quit",服务器程序退出当前数据交互，进入等待下一个客户端连接
    //  4、当服务器在数据交互过程中，客户端突然断开连接，服务器退出数据交互，进入等待下一个客户端连接
}
