#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <net/if.h>

#include <unistd.h>
// gcc client.c -o c
// ./c fe80::a8af:b821:7c06:375f 59999 
// argc : 3
// argv[0] ---> "./c"
// argv[1] ---> "fe80::a8af:b821:7c06:375f"
// argv[2] ---> "59999"
int senddata(int rws, const void *buf, size_t size, int timeout)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(rws, &set);

	struct timeval tm = {
		.tv_sec = timeout,
	};

	int ret = select(rws+1, NULL, &set, NULL, &tm);
	if(0 > ret){
		perror("select");
		return -1;
	}else if(0 == ret){
		printf("%d timeout.\n", rws);
		return 0;
	}

	ret = send(rws, buf, size, MSG_DONTWAIT | MSG_NOSIGNAL);
	if(0 >= ret){
		printf("link off.\n");
	}
	
	return ret;
}

int main(int argc, char *argv[])
{
	char *ipstr = "fe80::a8af:b821:7c06:375f";
	ushort port = 59999;

	if(3 == argc){
		ipstr = argv[1];
		port  = atoi(argv[2]);
	}

	int s = socket(AF_INET6, SOCK_STREAM, 0);
	if(0 > s){
		perror("socket");
		return -1;
	}

	struct sockaddr_in6 s_addr = {
		.sin6_family	= AF_INET6,
		.sin6_port	= htons(port),
		.sin6_scope_id	= if_nametoindex("ens33"),
	};
	if(inet_pton(AF_INET6, ipstr, s_addr.sin6_addr.s6_addr) <= 0){
		printf("ip is invalid.\n");
		goto ERR_STEP;		
	}
	socklen_t s_len = sizeof(s_addr);

	if(0 > connect(s, (struct sockaddr *)&s_addr, s_len)){
		perror("connect");
		goto ERR_STEP;		
	}

	printf("connected.\n");

#define MAX 64
	char filename[MAX];
	if(!fgets(filename, MAX, stdin)){
		goto ERR_STEP;
	}
	int size = strlen(filename);
	filename[size-1]=0;
	char req[MAX];
	snprintf(req, MAX, "G%s", filename);

	if(MAX != send(s, req, MAX, MSG_NOSIGNAL)){
		printf("link off.\n");
		goto ERR_STEP;
	}
	printf("snd req done.\n");

	if(4 != recv(s, &size, 4, MSG_WAITALL)){
		printf("recv file size fail.\n");
		goto ERR_STEP;
	}
	size = ntohl(size);

	FILE* fp = fopen("1.txt", "w");
	if(NULL == fp){
		goto ERR_STEP;
	}

	int num;
	while(size){
	#define BUFMAX 128
		char buffer[BUFMAX];
		if(size > BUFMAX){
			num = recv(s, buffer, BUFMAX, 0);
		}else{
			num = recv(s, buffer, size, 0);
		}

		if(0 >= num){
			printf("link off.\n");
			break;
		}

		fwrite(buffer, num, 1, fp);

		size -= num;
	}

	fclose(fp);

ERR_STEP:
	close(s);
	printf("client is leave.\n");
}
