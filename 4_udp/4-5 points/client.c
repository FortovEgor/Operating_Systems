#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSIZE 2048

void error_handler(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char **argv) {
    int client_sock, port_num, server_len, send_len, recv_len;
    char buffer[BUFSIZE];
    struct sockaddr_in server_addr;

    if (argc != 4) {
        printf("Usage: %s <server_ip> <port> <shopping_list>\n", argv[0]);
        exit(1);
    }

    port_num = atoi(argv[2]);

    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error_handler("Failed to create socket.");
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(port_num);

    // Отправляем список покупок на сервер
    server_len = sizeof(server_addr);
    if (sendto(client_sock, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&server_addr, server_len) < 0) {
        error_handler("Failed to send packet.");
    }

    printf("Sent shopping list to server.\n");

    // Ждем подтверждения от сервера
    memset(buffer, 0, BUFSIZE);
    if ((recv_len = recvfrom(client_sock, buffer, BUFSIZE, 0, NULL, NULL)) < 0) {
        error_handler("Failed to receive packet.");
    }

    printf("Received confirmation from server: %s\n", buffer);

    return 0;
}
