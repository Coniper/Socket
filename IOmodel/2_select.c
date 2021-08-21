/*******************************************************
  > File Name: 2_select.c
  > Author:Coniper
  > Describe: 
  > Created Time: 2021年08月20日
 *******************************************************/
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#define BFSZ 128

int main(int argc, char *argv[])
{
    //打开鼠标的设备文件
    int fd = open("/dev/input/mouse0", O_RDONLY);
    if(-1 == fd)
    {
        perror("open");
        return -1;
    }

    //使用select机制
    fd_set rfd_1, rfd_2;    //1、建立要检测的描述符集(表)
    FD_ZERO(&rfd_1);          //将 rfd 表中的数据全部清零 否则导致不可预期的后果
    FD_SET(0, &rfd_1);        //将 标准输入文件描述符 放入要检测的集合(表)中
    FD_SET(fd, &rfd_1);       //将 鼠标 的文件描述符 放入表中

    int nfds = fd + 1;

    while(1)
    {
        rfd_2 = rfd_1;  //select每次检测到响应后，当前文件描述符仍为 1， 则其他会被置 0，故下一次循环需要一张新表

        //select()函数检测 rfd集合 中有那些描述符可读，当集合中有一个描述符可读，该函数立马返回成功标志
        int ret = select(nfds, &rfd_2, NULL, NULL, NULL);
        if(0 > ret)
        {
            perror("select");
            return -1;
        }
        else
        {
            //判断 可读的文件符 为哪一个
            //FD_ISSET 函数判断 rfd中 可读文件描述符是否是  0， 如果是 则返回真 为(1)
            if(FD_ISSET(0, &rfd_2))
            {
                char buf[BFSZ] = "";
                fgets(buf ,sizeof(buf), stdin);
                printf("keyboard: %s\n", buf);
            }
            //FD_ISSET 函数判断 rfd中 可读文件描述符是否是 fd， 如果是 则返回真 为(1)
            else if(FD_ISSET(fd, &rfd_2))
            {
                char buf[BFSZ] = "";
                read(fd, buf, sizeof(buf));
                printf("mouse: %s\n", buf);
            }
        }
    }

    close(fd);

    return 0;
}

