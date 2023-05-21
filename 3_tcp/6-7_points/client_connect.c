#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_ITEM_NAME_SIZE 20   // Максимальная длина названия товара

int main(int argc, char* argv[]) {
    int port, sock, server_reply, curr_pos;
    struct sockaddr_in server_addr;
    char buffer[MAX_ITEM_NAME_SIZE];
    
    // Парсинг аргументов командной строки
    if (argc < 4) {
        printf("Usage: %s <IP address> <port> <shopping list>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    port = atoi(argv[2]);
    
    // Создание клиентского сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Установка адреса сервера и порта
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(port);
    
    // Подключение к серверу
    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        printf("Error in connect");
        exit(EXIT_FAILURE);
    }
    
    // Отправка списка покупок серверу
    for (curr_pos = 0; curr_pos < argc - 3; curr_pos++) {
        strcpy(buffer, argv[curr_pos + 3]);
        send(sock, buffer, MAX_ITEM_NAME_SIZE, 0);
        
        // Получение ответа сервера
        server_reply = 0;
        recv(sock, &server_reply, sizeof(int), 0);
        if (server_reply) {
            printf("Buy %s\n", buffer);
        }
        else {
            printf("Not found %s\n", buffer);
        }
    }
    
    // Закрытие клиентского сокета
    close(sock);
    
    return 0;
}
