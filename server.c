#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define MYPORT  8887
#define QUEUE   20
#define BUFFER_SIZE 1024

char ans[BUFFER_SIZE];

void _itoa(int n) {
    char ans2[BUFFER_SIZE];
    int l = 0;
    while (n) {
        ans2[l++] = n % 10 + '0';
        n /= 10;
    }
    int l2 = 0;
    while (l) {
        ans[l2++] = ans2[--l];
    }
    ans[l2++] = '\n';
    ans[l2] = '\0';
}

int main() {
    ///定义sockfd
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

    ///定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    ///bind，成功返回0，出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1) {
        perror("bind");
        exit(1);
    }

    ///listen，成功返回0，出错返回-1
    if(listen(server_sockfd,QUEUE) == -1) {
        perror("listen");
        exit(1);
    }

    ///客户端套接字
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    ///成功返回非负描述字，出错返回-1
    int conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
    if(conn<0) {
        perror("connect");
        exit(1);
    }

    while(1) {
        memset(buffer,0,sizeof(buffer));
        recv(conn, buffer, sizeof(buffer),0);
        if(strcmp(buffer,"q\n")==0)
            break;
        int a = 0;
        int i, bufflen = strlen(buffer);
        for (i = 2; i < bufflen - 1; i++) {
            if (buffer[i] != ',') {
                a = a * 10 + buffer[i] - '0';
            } else {
                i++;
                break;
            }
        }
        int b = 0;
        for (; i < bufflen - 1; i++) {
            b = b * 10 + buffer[i] - '0';
        }
        _itoa(a+b);
        fputs(buffer, stdout);
        send(conn, ans, strlen(ans), 0);
    }
    close(conn);
    close(server_sockfd);
    return 0;
}

