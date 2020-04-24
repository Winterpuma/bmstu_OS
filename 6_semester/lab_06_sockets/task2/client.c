#include "includes.h"

int main(void)
{
    // Establishing connection
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket failed\n");
        return sock;
    }

    struct hostent* host = gethostbyname(SOCK_ADDR); 
    if (!host)
    {
        perror("gethostbyname failed\n ");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr = *((struct in_addr*) host->h_addr_list[0]);

    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        perror("connect failed\n");
        return EXIT_FAILURE;
    }

    // Sending messages
    int pid = getpid();
        
    char msg[MSG_LEN];
    for (int i = 0; i < 4; i++)
    {
        memset(msg, 0, MSG_LEN);
        sprintf(msg, "Hello #%d from %d", i, pid);
        
        if (send(sock, msg, strlen(msg), 0) < 0)
        {
            perror("send failed: ");
            return EXIT_FAILURE;
        }
        
        printf("[%d] sent msg #%d: %s\n", pid, i, msg);
        sleep(3);
    }

    printf("Client [%d] terminated.\n", pid);
    return 0;
}
