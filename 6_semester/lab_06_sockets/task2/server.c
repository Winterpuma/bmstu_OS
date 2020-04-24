#include "includes.h"

#include <signal.h>
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS] = { 0 };

void newConnectionHandler(unsigned int fd)
{
    struct sockaddr_in addr;
    int addrSize = sizeof(addr);

    int incom = accept(fd, (struct sockaddr*) &addr, (socklen_t*) &addrSize);
    if (incom < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] == 0)
        {
            clients[i] = incom;
            break;
        }
    }
    
    printf("\nNew connection\nClient #%d: %s:%d\n\n", 
                            i, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
}

void clientHandler(unsigned int fd, unsigned int client_id)
{
    char msg[MSG_LEN];
    memset(msg, 0, MSG_LEN);

    struct sockaddr_in addr;
    int addrSize = sizeof(addr);

    int recvSize = recv(fd, msg, MSG_LEN, 0);
    if (recvSize == 0)
    {
        getpeername(fd, (struct sockaddr*) &addr, (socklen_t*) &addrSize);
        printf("\nClient #%d disconnected\n\n", client_id);
        close(fd);
        clients[client_id] = 0;
    }
    else
    {
        msg[recvSize] = '\0';
        printf("Message from client #%d: %s\n", client_id, msg);
    }
}


int main(void)
{
    // Establishing connection
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket failed\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    {
        perror("bind failed\n");
        return EXIT_FAILURE;
    }

    if (listen(sock, 3) < 0)
    {
        perror("listen failed ");
        return EXIT_FAILURE;
    }
        
    // Handling requests
    printf("Server configured. Listening on port %d.\n", SOCK_PORT);

    while (1)
    {
        // Fill sockets
        int max_fd = sock;
        
        fd_set set; 
        FD_ZERO(&set);
        FD_SET(sock, &set);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] > 0)
                FD_SET(clients[i], &set);

            max_fd = (clients[i] > max_fd) ? (clients[i]) : (max_fd);
        }
        
        // Wait for event in one of sockets 
        struct timeval timeout = {15, 0}; // 15 sec
        int select_ret = select(max_fd + 1, &set, NULL, NULL, &timeout);
        if (select_ret == 0)
        {
            printf("\nServer closed connection by timeout.\n\n");
            return 0;
        }
        else if (select_ret < 0)
        {
            perror("select failed");
            return EXIT_FAILURE;
        }
        
        // Checking for updates
        //      Connections
        if (FD_ISSET(sock, &set))
            newConnectionHandler(sock);

        //      Messages
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients[i];
            if ((fd > 0) && FD_ISSET(fd, &set))
                clientHandler(fd, i);
        }
    }

    return 0;
}
