#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define SOCK_NAME "mysocket.s"

int main()
{
    int sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        perror("socket failed");
        return EXIT_FAILURE;
    }
    
    struct sockaddr srvr_name;
    srvr_name.sa_family = AF_UNIX;
    strcpy(srvr_name.sa_data, SOCK_NAME);
    
    char buf[100];
    scanf("%99s", buf);
    sendto(sock_fd, buf,strlen(buf), 0, &srvr_name, strlen(srvr_name.sa_data) + sizeof(srvr_name.sa_family));
    
    printf("Client sent: %s\n", buf);
    return 0;
}
