#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 9999
#define MAX_BUFF_SIZE 1024

int main(int argc, char *argv[]) {
    int client_socket;
    struct sockaddr_in server_address;

    // создание сокета
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("ERROR: Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    // настройка адреса сервера
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_address.sin_port = htons(SERVER_PORT);

    // подключение к серверу
    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("ERROR: Failed to connect to server.");
        exit(EXIT_FAILURE);
    }

    // добавление покупателя в очередь отдела 1
    char message[MAX_BUFF_SIZE] = {"1a"};
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("ERROR: Failed to send message to server.");
        exit(EXIT_FAILURE);
    }
    printf("Added customer to department 1\n");

    // получение списка товаров отдела 1
    memset(message, 0, sizeof(message));
    strcat(message, "1g");
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("ERROR: Failed to send message to server.");
        exit(EXIT_FAILURE);
    }
    char server_response[MAX_BUFF_SIZE] = {0};
    if (recv(client_socket, server_response, MAX_BUFF_SIZE, 0) == -1) {
        perror("ERROR: Failed to receive message from server.");
        exit(EXIT_FAILURE);
    }
    printf("List of items for department 1: %s\n", server_response);

    // удаление покупателя из очереди отдела 1
    memset(message, 0, sizeof(message));
    strcat(message, "1r");
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("ERROR: Failed to send message to server.");
        exit(EXIT_FAILURE);
    }
    printf("Removed customer from department 1\n");

    close(client_socket); // закрываем соединение
    return 0;
}