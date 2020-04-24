#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#define SOCK_NAME "mysocket.s"

int sock_fd;

void close_sock(int sock_fd, char *name)
{
    close(sock_fd);
    unlink(name);
}


void sigint_handler(int signum)
{
    close_sock(sock_fd, SOCK_NAME);
    printf("\nSocket was closed due to ctrl+c!\n");
    printf("Server will be stopped.\n");
    exit(0);
}


int main()
{
    sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        perror("socket failed");
        return EXIT_FAILURE;
    }
    
    struct sockaddr srvr_name;
    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);
    if(bind(sock_fd,&srvr_name, strlen(srvr_name.sa_data)+sizeof(srvr_name.sa_family)) < 0)
    {
        perror("bind failed");
        return EXIT_FAILURE;
    }
    
    signal(SIGINT, sigint_handler);
    printf("Server is listening.\nTo stop server press ctrl+c or send \"stop\" via client.\n");
    
    char buf[100];
    while (strcmp(buf, "stop"))
    {
        int bytes = recv(sock_fd, buf, sizeof(buf), 0);
        if (bytes <= 0)
        {
            perror("recv failed");
            close_sock(sock_fd, SOCK_NAME);
            return EXIT_FAILURE;
        }   
        buf[bytes] = 0;
        printf("Server read: %s\n", buf);
    }
    
    printf("Server stopped listening\n");
    close_sock(sock_fd, SOCK_NAME);
    printf("Socket closed\n");
    return 0;
}
