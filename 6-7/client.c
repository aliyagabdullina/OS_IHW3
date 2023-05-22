#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void update_system_status(const char *status) {
    printf("System Status: %s\n", status);
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &(server_address.sin_addr)) <= 0) {
        perror("Неверный адрес");
        exit(EXIT_FAILURE);
    }

    if(connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Соединение не установлено");
        exit(EXIT_FAILURE);
    }
    update_system_status("Подключение к серверу успешно");

// Обработка запросов к серверу
   int r = rand() % 2;
    if (r == 0) {
        const char *message = "Булавка кривая";
    } else {
        const char *message = "Булавка прямая";
    }
    write(client_socket, message, strlen(message));

    // Чтение данных от сервера
    char buffer[256];
    ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Получены данные от сервера: %s\n", buffer);
    }


close(client_socket);

return 0;
}
