#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_LIST 10
#define MAX_ITEMS 5

typedef struct {
    int id;
    int items[MAX_ITEMS];
    int len;
    int index;
} Customer;

typedef struct {
    int id;
    int items[MAX_ITEMS];
    int len;
} Department;

typedef struct {
    Customer *customer;
    Department *department;
} Request;

Department department1 = {1, {1, 2, 3}, 3};
Department department2 = {2, {4, 5}, 2};
Customer customers[MAX_LIST];
Request requests[MAX_LIST];
pthread_mutex_t lock;
int num_customers = 0;
int num_requests = 0;
int next_customer_id = 0;

void *department_thread(void *dep_ptr) {
    Department *dep = (Department *) dep_ptr;
    while (1) {
        pthread_mutex_lock(&lock);
        if (num_requests > 0 && requests[0].department->id == dep->id) {
            Request request = requests[0];
            for (int i = 0; i < request.customer->len; i++) {
                int item = request.customer->items[i];
                for (int j = 0; j < dep->len; j++) {
                    if (item == dep->items[j]) {
                        printf("Customer %d purchases item %d from department %d\n", request.customer->id, item, dep->id);
                        usleep(1000000);
                    }
                }
            }
            for (int i = 1; i < num_requests; i++) {
                requests[i - 1] = requests[i];
            }
            num_requests--;
        }
        pthread_mutex_unlock(&lock);
        usleep(1000000);
    }
}

void *customer_thread(void *sock_ptr) {
    int sock = *(int *) sock_ptr;
    Customer customer = customers[next_customer_id++];
    write(sock, &customer, sizeof(Customer));
    printf("Customer %d enters the store\n", customer.id);
    while (1) {
        Request request;
        if (read(sock, &request, sizeof(Request)) > 0) {
            pthread_mutex_lock(&lock);
            requests[num_requests++] = request;
            pthread_mutex_unlock(&lock);
            printf("Customer %d requests items from department %d\n", customer.id, request.department->id);
            while (request.customer->index < request.customer->len &&
                   request.customer->items[request.customer->index] != 0) {
                sleep(1);
            }
            if (request.customer->index >= request.customer->len) {
                break;
            }
            write(sock, &request, sizeof(Request));
            request.customer->index++;
        }
    }
    printf("Customer %d leaves the store\n", customer.id);
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip_addr> <port>\n", argv[0]);
        return 1;
    }
    pthread_t department1_thread, department2_thread;
    int server_sock, client_sock, opt = 1;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Failed to create socket");
        return 1;
    }
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Failed to set socket options");
        return 1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));
    if (bind(server_sock, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Failed to bind socket");
        return 1;
    }
    if (listen(server_sock, MAX_LIST) == -1) {
        perror("Failed to listen on socket");
        return 1;
    }
    pthread_create(&department1_thread, NULL, department_thread, &department1);
    pthread_create(&department2_thread, NULL, department_thread, &department2);
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *) &client_address, &client_address_len);
        if (client_sock ==-1) {
            perror("Failed to accept client connection");
            continue;
        }
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, customer_thread, &client_sock);
    }
    close(server_sock);
    return 0;
}
