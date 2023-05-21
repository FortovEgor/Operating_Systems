#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct {
    int id;
    int items[5];
    int len;
    int index;
} Customer;

typedef struct {
    int id;
    int items[5];
    int len;
} Department;

typedef struct {
    Customer *customer;
    Department *department;
} Request;

Customer customer = {0, {4, 1, 3, 2, 5}, 5, 0};
Department department1 = {1, {1, 2, 3}, 3};
Department department2 = {2, {4, 5}, 2};

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <ip_addr> <port>\n", argv[0]);
        return 1;
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));
    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Failed to connect to server");
        return 1;
    }
    write(sock, &customer, sizeof(Customer));
    printf("Customer %d enters the store\n", customer.id);
    while (customer.index < customer.len && customer.items[customer.index] != 0) {
        Department department;
        if (customer.items[customer.index] == 1) {
            department = department1;
        } else {
            department = department2;
        }
        Request request = {&customer, &department};
        write(sock, &request, sizeof(Request));
        printf("Customer %d requests items from department %d\n", customer.id, department.id);
        usleep(1000000);
        if (read(sock, &request, sizeof(Request)) > 0) {
            customer = *request.customer;
            printf("Customer %d purchases item %d from department %d\n", customer.id, customer.items[customer.index - 1], department.id);
        }
    }
    printf("Customer %d leaves the store\n", customer.id);
    close(sock);
    return 0;
}
