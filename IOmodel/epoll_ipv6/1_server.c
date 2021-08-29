/*******************************************************
	> File Name: 6_epoll.c
	> Author:Coniper
	> Describe: 
	> Created Time: 2021年08月23日
 *******************************************************/
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/stat.h>

#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

//初始化监听套接字
int server_init(ushort port, int backlog)
{
	int s = socket(AF_INET6, SOCK_STREAM, 0);
	if(0 > s){
		perror("socket");
		return -1;
	}

	struct sockaddr_in6 addr = {
		.sin6_family	= AF_INET6,
		.sin6_port	= htons(port),
		.sin6_addr	= in6addr_any,
	};
	socklen_t len = sizeof(addr);
	if(0 > bind(s, (struct sockaddr *)&addr, len)){
		perror("bind");
		goto ERR_STEP;
	}

	if(0 > listen(s, backlog)){
		perror("listen");
		goto ERR_STEP;
	}

	return s;

ERR_STEP:
	close(s);
	return -1;
}

//设置指定文件描述符为非阻塞工作模式
int setNoblock(int fd)
{
	//得到指定文件描述符的标志
	int flags = fcntl(fd, F_GETFL);
	if(0 > flags){
		perror("fcntl");
		return -1;
	}

	//在原有标志的基础上追加一个非阻塞标志
	flags |= O_NONBLOCK;

	//把标志设置回文件描述符
	if(0 > fcntl(fd, F_SETFL, flags)){
		perror("fcntl");
		return -1;
	}

	return 0;
}

//读出连接
int getlink(int efd, int s)
{
	struct sockaddr_in6 addr;
	socklen_t len = sizeof(addr);
	//读出连接
	int rws = accept(s, (struct sockaddr *)&addr, &len);
	if(0 > rws){
		perror("accept");
		return -1;
	}

	char ipstr[INET6_ADDRSTRLEN];
	printf("socket %d: client %s : %u  entry.\n", rws,
		inet_ntop(AF_INET6, &addr.sin6_addr, ipstr, INET6_ADDRSTRLEN),
		ntohs(addr.sin6_port));	

	//事件结构
	struct epoll_event evt = {
		.events = EPOLLIN,//监测读事件
		.data	= {	//用户数据
			.fd = rws,//至少要存监测对象（这里放的是什么，epoll_wait读出就是什么）
		},
	};

	//向efd指定的epoll链表中添加节点
	//其中 rws:被添加的监测对象，evt被添加的监测事件结构
	if(0 > epoll_ctl(efd, EPOLL_CTL_ADD, rws, &evt)){
		perror("epoll_ctl - EPOLL_CTL_ADD");
		close(rws); //添加不成功则应该关闭连接
		return -1;
	}

	return 0;
}

//接收数据
int recvdata(int efd, int rws)
{
#define MAX 512
	char buf[MAX];
	//从接收缓存读出数据：如果连接断开会失败
	int num = recv(rws, buf, MAX-1, 0);
	if(0 >= num){ //连接断开
		//从epoll链表中删除指定对象
		epoll_ctl(efd, EPOLL_CTL_DEL, rws, NULL);
		close(rws); //接收不成功则应该关闭连接
		printf("%d is leave.\n", rws);
		return -1;
	}
	buf[num]= 0;
	printf("socket %d, recv %dbytes: %s\n", rws, num, buf);
	return num;
}


int main()
{
	//创建epoll
	int efd = epoll_create(1);
	if(0 > efd){
		perror("epoll_create");
		return -1;
	}

	//初始化监听套接字
	int s = server_init(59999, 10);
	if(0 > s){
		goto ERR_STEP;
	}
	printf("epoll = %d; listen = %d\n", efd, s);

	//设置监听套接字为非阻塞
	if(0 > setNoblock(s)){
		goto ERR_STEP1;
	}

	//刚开始只有监听套接字被监测，把监听套接字插入到epoll链表
	//监测事件结构
	struct epoll_event evt = {
		.events = EPOLLIN,//监测事件
		.data	= {	//用户数据
			.fd = s,//至少要存监测对象
		},
	};
	//s:监测对象，evt是监测事件结构
	if(0 > epoll_ctl(efd, EPOLL_CTL_ADD, s, &evt)){
		perror("epoll_ctl - EPOLL_CTL_ADD");
		goto ERR_STEP1;
	}
	
	while(1){
		printf("wait for a client ...\n");
	#define EMAX 6
		struct epoll_event evts[EMAX];
		//从efd[指定epoll]监测的对象及事件等到结果，并到保存evts中，返回值是实际发生的数量
		int num = epoll_wait(efd, evts, EMAX, 3000);
		if(0 > num){
			perror("epoll_wait");
			break;
		}else if(0 == num){
			printf("timeout.\n");
			continue;
		}

		//每个事件处理一次:因为监测的都是读事件EPOLLIN，故此不用测试事件
		for(int i = 0; i < num; i++){
			//连接处理和接收数据都需要文件描述符，
			//故此必须在添加事件节点时，在事件结构的用户数据放文件描述
			if(s == evts[i].data.fd){ //是有连接了
				getlink(efd, s);
			}else{
				recvdata(efd, evts[i].data.fd);
			}
		}
	}
	
ERR_STEP1:
	close(s);

ERR_STEP:
	close(efd);
	printf("server is leave.\n");
	return 0;
}

