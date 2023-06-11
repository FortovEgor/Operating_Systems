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
    int server_sock, client_sock, port_num, client_len, recv_len;
    char buffer[BUFSIZE];
    struct sockaddr_in server_addr, client_addr;

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    port_num = atoi(argv[1]);

    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error_handler("Failed to create socket.");
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port_num);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error_handler("Failed to bind socket.");
    }

    while (1) {
        memset(buffer, 0, BUFSIZE);
        client_len = sizeof(client_addr);

        if ((recv_len = recvfrom(server_sock, buffer, BUFSIZE, 0, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            error_handler("Failed to receive packet.");
        }

        // Код для обработки списка покупок и вывода информации в терминал
        printf("Received packet from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("Shopping list: %s\n", buffer);
        printf("Processing shopping list...\n");
        sleep(5);
        printf("Shopping list processed!\n");

        // Отправляем подтверждение клиенту
        if (sendto(server_sock, "Processed", strlen("Processed"), 0, (struct sockaddr *)&client_addr, client_len) < 0) {
            error_handler("Failed to send confirmation packet.");
        }
    }

    return 0;
}
