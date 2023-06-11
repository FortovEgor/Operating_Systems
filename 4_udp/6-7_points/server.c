#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFF_SIZE 1024

typedef struct {
    char item[50];
    char department[50];
} ShoppingList;

int main(int argc, char *argv[]) {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    int client_address_length = sizeof(client_address);

    ShoppingList shopping_lists[2][MAX_BUFF_SIZE]; // два отдела, каждый со своим списком товаров
    int current_customers[2] = {0}; // количество текущих покупателей у каждого отдела
    int max_customers[2] = {1}; // максимальное количество покупателей у каждого отдела

    // заполнение списков товаров
    strcpy(shopping_lists[0][0].item, "bread");
    strcpy(shopping_lists[0][0].department, "bakery");
    strcpy(shopping_lists[0][1].item, "milk");
    strcpy(shopping_lists[0][1].department, "dairy");

    strcpy(shopping_lists[1][0].item, "apple");
    strcpy(shopping_lists[1][0].department, "fruits");
    strcpy(shopping_lists[1][1].item, "orange");
    strcpy(shopping_lists[1][1].department, "fruits");

    // создание сокета
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("ERROR: Failed to create socket.");
        exit(EXIT_FAILURE);
    }

    // настройка адреса сервера
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(9999);

    // связывание сокета с адресом сервера
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("ERROR: Failed to bind socket.");
        exit(EXIT_FAILURE);
    }

    // прослушивание входящих соединений
    if (listen(server_socket, 2) == -1) {
        perror("ERROR: Failed to listen for incoming connections.");
        exit(EXIT_FAILURE);
    }

    // обработка входящих соединений и сообщений
    while (1) {
        int client_socket;
        char client_message[MAX_BUFF_SIZE] = {0};

        // принятие входящего соединения
        if ((client_socket = accept(server_socket, (struct sockaddr *) &client_address, (socklen_t *) &client_address_length)) == -1) {
            perror("ERROR: Failed to accept incoming connection.");
            exit(EXIT_FAILURE);
        }

        // чтение сообщения от клиента
        if (recv(client_socket, client_message, MAX_BUFF_SIZE, 0) == -1) {
            perror("ERROR: Failed to receive message from client.");
            exit(EXIT_FAILURE);
        }

        // обработка сообщения от клиента
        int department = client_message[0] - '0'; // номер отдела
        switch (client_message[1]) {
            case 'a': // добавление покупателя в очередь
                if (current_customers[department] < max_customers[department]) { // в отделе есть место для нового покупателя
                    current_customers[department]++;
                    printf("Added customer to department %d.\n", department);
                } else { // отдел полон, новый покупатель засыпает
                    printf("Department %d is full. Customer falls asleep.\n", department);
                    sleep(5); // засыпаем на 5 секунд
                }
                break;
            case 'r': // удаление покупателя из очереди
                current_customers[department]--;
                printf("Removed customer from department %d.\n", department);
                break;
            case 'g': // получение списка товаров
                char response[MAX_BUFF_SIZE] = {0};
                int i;
                for (i = 0; i < current_customers[department]; i++) {
                    strcat(response, shopping_lists[department][i].item);
                    strcat(response, " (");
                    strcat(response, shopping_lists[department][i].department);
                    strcat(response, ")");
                    if (i < current_customers[department] - 1) {
                        strcat(response, ", ");
                    }
                }
                // отправка ответа клиенту
                if (send(client_socket, response, strlen(response), 0) == -1) {
                    perror("ERROR: Failed to send message to client.");
                    exit(EXIT_FAILURE);
                }
                printf("Sent list of items for department %d: %s\n", department, response);
                break;
        }

        close(client_socket); // закрываем соединение
    }

    close(server_socket); // закрываем серверный сокет
    return 0;
}