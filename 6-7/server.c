#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

void update_system_status(const char *status) {
    printf("System Status: %s\n", status);
}

int main() {
    int server_fd, new_socket, observer_socket;
    struct sockaddr_in address, observer_address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    update_system_status("Server is running");

    if ((observer_socket = accept(server_fd, (struct sockaddr *)&observer_address, (socklen_t*)&addrlen)) < 0) {
        perror("Observer accept failed");
        exit(EXIT_FAILURE);
    }

    update_system_status("Observer connected");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Обработка запросов от клиента
      
       printf("Получено подключение от клиента\n");

        // Создание отдельного процесса для обработки клиента
        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
            close(client_socket);
            exit(0);
    } else {
        close(client_socket);
    }

        // Отправка информации наблюдателю
        char status_message[100];
        snprintf(status_message, sizeof(status_message), "New client connected: %d", new_socket);
        send(observer_socket, status_message, strlen(status_message), 0);

        close(new_socket);
    }

    close(observer_socket);
    close(server_fd);

    return 0;
}
