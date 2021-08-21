/*******************************************************
	> File Name: 1_nonblock.c
	> Author:Coniper
	> Describe: 
	> Created Time: 2021年08月20日
 *******************************************************/
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>

#define BUFSZ 128

int main(int argc, char *argv[])
{
    char buf[BUFSZ] = "";

    int flag = fcntl( 0, F_GETFL, 0);   //F_GETFL: 获得文件描述符的原有标志
    flag |= O_NONBLOCK;                 //将新的标志放入 flag 中        //O_NONBLOCK: 非阻塞模式
    fcntl( 0, F_SETFL, flag);           //重新设置文件描述法新的标志

    printf("input: ");
    fgets(buf, sizeof(buf), stdin);

    printf("buf: %s\n", buf);

    return 0;
}

