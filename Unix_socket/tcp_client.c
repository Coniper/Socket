

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/un.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    //创建套接字
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ( -1 == sockfd){
        perror("socket");
        return -1;
    }
    printf("sockfd: %d\n", sockfd);

    //绑定（可选）
    //3、连接到服务器
    struct sockaddr_un saddr;
    saddr.sun_family        = AF_UNIX;
    //当客户端运行时，该套接字文件一定是存在的，只需要检测是否用可读科协权限
    if ( 0== access("/tmp/mysocket", R_OK | W_OK)){
        strcpy(saddr.sun_path, "/tmp/mysocket");
    }

    if ( -1 == connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr))){
        perror("connect");
        return -1;
    }
    char buf[128] = {0};
    //数据交互
    while(1){
        fgets(buf, sizeof(buf), stdin);
        send(sockfd, buf, strlen(buf), 0);
        memset(buf, 0, sizeof(buf));
        int ret = recv(sockfd, buf, sizeof(buf), 0);
        if ( -1 == ret ){
            perror("recv");
            break;
        } else if ( 0 == ret ){
            printf("服务器网络异常\n");
            break;
        } else {
            printf("recv: %s\n",  buf);
        } 
    }
    close(sockfd);
}
