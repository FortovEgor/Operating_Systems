#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_LIST_SIZE 50    // Максимальный размер списка покупок
#define MAX_ITEM_NAME_SIZE 20   // Максимальная длина названия товара
#define SHOP_ITEMS_COUNT 10 // Общее количество товаров в магазине
#define WORK_DAY_HOURS 8    // Количество рабочих часов в день
#define DEFAULT_PORT 54321  // Порт по умолчанию

// Структура для представления покупателя
typedef struct {
    int id; // Идентификатор покупателя
    int curr_pos; // Текущее положение покупателя в списке
    int list_size; // Размер списка покупок
    char shopping_list[MAX_LIST_SIZE][MAX_ITEM_NAME_SIZE]; // Список покупок
} Customer;

// Структура для представления продавца
typedef struct {
    int id; // Идентификатор продавца
    bool is_sleeping; // Состояние продавца (сон/бодрствование)
} Salesman;

// Функция для генерации случайного списка покупок для покупателя
void generate_shopping_list(Customer* customer) {
    int i, j, r, already_in_list;
    char shop_items[SHOP_ITEMS_COUNT][MAX_ITEM_NAME_SIZE] = {
        "apple", "banana", "bread", "milk", "cheese",
        "chicken", "potato", "tomato", "carrot", "orange"
    };
    
    // Генерируем случайный список покупок размером от 1 до MAX_LIST_SIZE
    customer->list_size = rand() % MAX_LIST_SIZE + 1;
    
    // Выбираем случайные товары из общего списка товаров
    for (i = 0; i < customer->list_size; i++) {
        already_in_list = 1;
        while (already_in_list) {
            r = rand() % SHOP_ITEMS_COUNT;
            already_in_list = 0;
            for (j = 0; j < i; j++) {
                if (strcmp(customer->shopping_list[j], shop_items[r]) == 0) {
                    already_in_list = 1;
                    break;
                }
            }
        }
        strcpy(customer->shopping_list[i], shop_items[r]);
    }
}

// Функция для обработки сообщений от клиентов
void handle_customer_message(int client_sock, Customer* customer, Salesman* salesmen) {
    char buffer[MAX_ITEM_NAME_SIZE];
    int recv_size;
    
    // Читаем название товара, которое покупатель хочет купить
    recv_size = recv(client_sock, buffer, MAX_ITEM_NAME_SIZE, 0);
    buffer[recv_size] = '\0';
    
    // Если это первый элемент в списке, то покупатель необходимо перевести к продавцу, обслуживающему этот отдел
    if (customer->curr_pos == 0) {
        send(client_sock, &salesmen[customer->list_size % 2].id, sizeof(int), 0);
    }
    
    usleep(500000); // Имитация времени, необходимого для перемещения покупателя между отделами
    
    // Если текущий элемент списка совпадает с запрошенным товаром, отправляем ответ "YES"
    if (strcmp(buffer, customer->shopping_list[customer->curr_pos]) == 0) {
        send(client_sock, "YES", 3, 0);
        customer->curr_pos++;
    }
    else {
        send(client_sock, "NO", 2, 0);
    }
}

// Функция для обработки сообщений от продавцов
void handle_salesman_message(int client_sock, Salesman* salesman, Customer* customers, int customers_count) {
    int i;
    
    // Если в отделе нет покупателей, то продавец засыпает
    if (customers_count == 0) {
        salesman->is_sleeping = true;
        return;
    }
    
    usleep(500000); // Имитация времени, необходимого для обслуживания покупателя
    
    // Обслуживаем первого покупателя в очереди
    for (i = 0; i < customers_count; i++) {
        if (customers[i].curr_pos == 0) {
            send(client_sock, customers[i].shopping_list[0], MAX_ITEM_NAME_SIZE, 0);
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    int port, i, j, server_sock, client_sock, salesman_socks[2], activity, max_sock, salesmen_count, customers_count;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    fd_set read_fds;
    time_t t;
    Customer customers[100];
    Salesman salesmen[2];
    
    // Инициализация генератора случайных чисел
    srand((unsigned) time(&t));
    
    // Парсинг аргументов командной строки
    port = argc > 1 ? atoi(argv[1]) : DEFAULT_PORT;
    
    // Создание серверного сокета
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Адрес сервера и порт
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Привязка сокета к адресу и порту
    bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr));
    
    // Перевод сокета в режим прослушивания
    listen(server_sock, 5);
    
    // Инициализация массива продавцов
    salesmen[0].id = 0;
    salesmen[0].is_sleeping = true;
    salesmen[1].id = 1;
    salesmen[1].is_sleeping = true;
    salesmen_count = 2;
    
    // Ожидание подключения клиентов
    printf("Waiting for clients...\n");
    while (1) {
        // Очистка множества файловых дескрипторов
        FD_ZERO(&read_fds);
        
        // Добавление серверного сокета в множество
        FD_SET(server_sock, &read_fds);
        max_sock = server_sock;
        
        // Добавление сокетов продавцов в множество
        for (i = 0; i < salesmen_count; i++) {
            if (!salesmen[i].is_sleeping) {
                FD_SET(salesman_socks[i], &read_fds);
                if (salesman_socks[i] > max_sock) {
                    max_sock = salesman_socks[i];
                }
            }
        }
        
        // Добавление сокетов клиентов в множество
        for (i = 0; i < customers_count; i++) {
            FD_SET(customers[i].id, &read_fds);
            if (customers[i].id > max_sock) {
                max_sock = customers[i].id;
            }
        }
        
        // Ожидание активности на одном из сокетов
        activity = select(max_sock + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            printf("Error in select");
            exit(EXIT_FAILURE);
        }
        
        // Проверка получения нового подключения на серверный сокет
        if (FD_ISSET(server_sock, &read_fds)) {
            addr_size = sizeof(client_addr);
            // Принимаем нового клиента
            client_sock = accept(server_sock, (struct sockaddr*) &client_addr, &addr_size);
            
            // Генерируем случайный список покупок для покупателя
            generate_shopping_list(&customers[customers_count]);
            customers[customers_count].id = client_sock;
            customers[customers_count].curr_pos = 0;
            
            printf("New customer #%d connected\n", customers[customers_count].id);
            customers_count++;
        }
        
        // Проверка получения сообщения на сокет продавца
        for (i = 0; i < salesmen_count; i++) {
            if (FD_ISSET(salesman_socks[i], &read_fds)) {
                handle_salesman_message(salesman_socks[i], &salesmen[i], customers, customers_count);
            }
        }
        
        // Проверка получения сообщения на сокет клиента
        for (i = 0; i < customers_count; i++) {
            if (FD_ISSET(customers[i].id, &read_fds)) {
                handle_customer_message(customers[i].id, &customers[i], salesmen);
            }
        }
        
        // Удаление завершивших работу клиентов
        for (i = 0; i < customers_count; i++) {
            if (customers[i].curr_pos == customers[i].list_size) {
                printf("Customer #%d has left the store\n", customers[i].id);
                close(customers[i].id);
                // Сдвиг оставшихся клиентов в массиве
                for (j = i; j < customers_count - 1; j++) {
                    customers[j] = customers[j+1];
                }
                customers_count--;
                i--;
            }
        }
        
        // Проверка необходимости добавления нового продавца
        if (customers_count >= 5 && salesmen_count < 2) {
            salesmen[salesmen_count].id = salesmen_count;
            
            // Создание сокета для нового продавца
            salesman_socks[salesmen_count] = socket(AF_INET, SOCK_STREAM, 0);
            
            // Установка адреса сервера и порта для нового продавца
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            server_addr.sin_port = htons(port);
            
            // Подключение нового продавца к серверу
            connect(salesman_socks[salesmen_count], (struct sockaddr*) &server_addr, sizeof(server_addr));
            
            printf("New salesman #%d has started working\n", salesmen[salesmen_count].id);
            
            salesmen_count++;
        }
        
        // Проверка окончания рабочего дня
        if (time(NULL) >= t + WORK_DAY_HOURS * 60 * 60) {
            printf("Working day has ended\n");
            break;
        }
    }
    
    // Закрытие сокетов клиентов
    for (i = 0; i < customers_count; i++) {
        close(customers[i].id);
    }
    
    // Закрытие сокетов продавцов
    for (i = 0; i < salesmen_count; i++) {
        close(salesman_socks[i]);
    }
    
    // Закрытие серверного сокета
    close(server_sock);
    
    return 0;
}