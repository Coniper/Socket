/*******************************************************
  > File Name: 4_poll.c
  > Author:Coniper
  > Describe: 
  > Created Time: 2021年08月20日
 *******************************************************/
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <poll.h>

#define BFSZ 128
#define PSZ 10

int main(int argc, char *argv[])
{
    //打开鼠标的设备文件
    int fd = open("/dev/input/mouse0", O_RDONLY);
    if(-1 == fd)
    {
        perror("open");
        return -1;
    }

    //定义结构体数组，用存储多个要检测的文件描述法和请求检测的事件
    struct pollfd fds[PSZ];
    fds[0].fd       = 0;       //设置要检测的文件描述符
    fds[0].events   = POLLIN;  //设置请求事件为 可读

    fds[1].fd       = fd;
    fds[1].events   = POLLIN;

    int fdcount = 2;
    struct timeval tm;

    //使用poll机制实现IO多路复用
    while(1)
    {
        tm.tv_sec = 2;

        //poll()函数检测集合中哪一个文件描述符就绪，那么就会将 描述符对应的结构体变量中的 revents 成员置位
        int ret = poll(fds, fdcount, 2);
        if(-1 == ret)
        {
            perror("poll");
            break;
        }
        else if(0 == ret)
        {
            printf("timeout\n");
            continue;
        }

#if 0
        //判断数组中哪一个成员的文件描述符准备就修，并且判断事件为 自己设置的事件
        if(fds[0].revents & POLLIN)     //常规比较少的判断方法 //&上POLLIN为真说明被置为了POLLIN
        {
            char buf[BFSZ] = "";
            //fgets(buf, sizeof(buf), 0);
            read(fds[0].fd, buf, sizeof(buf));
            printf("keyboard: %s\n", buf);
        }
        else if(fds[1].revents & POLLIN)
        {
            char buf[BFSZ] = "";
            read(fds[1].fd, buf, sizeof(buf));
            printf("mouse: %s", buf);
        }
    }
#else
    for(int i = 0; i < fdcount; i++)
    {   
        if(fds[i].revents & POLLIN)     //可以用switch
        {
            if(fds[i].fd == 0)
            {
                printf("keyboard\n");
                break;
            }
            else if(fds[i].fd == fd)
            {
                printf("mouse\n");
                break;
            }
        }
    }
}
#endif

close(fd);

return 0;
}

