#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 3

struct Pin {
    int id;
    int is_curved;
};

void handle_client(int client_socket) {
    // Обработка клиентского соединения
    struct Pin pin;
    pin.id = 1;
    pin.is_curved = 0;
    write(client_socket, &pin, sizeof(struct Pin));
  
    struct Pin received_pin;
    ssize_t bytes_read = read(client_socket, &received_pin, sizeof(struct Pin));
    if (bytes_read == sizeof(struct Pin)) {
        // Обработка полученной булавки
        if (received_pin.is_curved) {
            printf("Булавка %d кривая. Отбракована.\n", received_pin.id);
        } else {
            printf("Булавка %d передана следующему рабочему.\n", received_pin.id);
            write(next_client_socket, &received_pin, sizeof(struct Pin));
        }
    }
}

int main() {
    int server_socket, client_socket, i;
    struct sockaddr_in server_address, client_address;
    socklen_t address_size = sizeof(client_address);

    // Создание сокета сервера
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Ошибка при создании сокета сервера");
        exit(1);
    }

    // Настройка адреса сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Привязка сокета к адресу сервера
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Ошибка при привязке сокета к адресу сервера");
        exit(1);
    }

    // Ожидание подключений от клиентов
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Ошибка при ожидании подключений");
        exit(1);
    }

    printf("Сервер запущен. Ожидание подключений...\n");

    // Обработка подключений от клиентов
    for (i = 0; i < MAX_CLIENTS; i++) {
        // Принятие подключения от клиента
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_size);
        if (client_socket == -1) {
            perror("Ошибка при принятии подключения");
            exit(1);
        }

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
}

// Закрытие сокета сервера
close(server_socket);

return 0;
}
