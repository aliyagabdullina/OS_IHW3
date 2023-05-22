# OS_IHW3
# Габдуллина Алия Маратовна БПИ214
# Вариант 26. Условие задачи: 
Задача о производстве булавок. В цехе по заточке булавок все
необходимые операции осуществляются тремя рабочими. Первый
из них берет булавку и проверяет ее на предмет кривизны. Если
булавка не кривая, то рабочий передает ее своему напарнику. Иначе выбрасывает в отбраковку. Напарник осуществляет собственно
заточку и передает заточенную булавку третьему рабочему, который осуществляет контроль качества операции бракуя булавку или
отдавая на упаковку. Требуется создать приложение, моделирующее работу цеха. При решении использовать парадигму
«производитель-потребитель». Следует учесть, что каждая из операций выполняется за случайное время которое не связано с конкретным рабочим. Возможны различные способы реализации передачи (на усмотрение разработчика). Либо непосредственно по одной булавке, либо через ящики, в которых буферизируется некоторое конечное количество булавок.


Задание выполнено на 7

# Код на 4-5: 
Программа создает один родительский и три дочерних процесса, каждый из которых выполняет свою функцию:

0. Сервер - цех
1. Клиенты - рабочие, первый проверяет булавку на кривизну, второй затачивает булавку, третий контролирует качество булавки.

Логика работы программы: Запускается сервер, который будет слушать порт (8080) и ждать подключений от клиентов.
Запускаются клиенты, которые представляют рабочих. Каждый клиент будет подключаться к соответствующему серверу.
Клиенты проверяют булавки на кривизну и отправляют булавки на серверы для выполнения операций. Булавки передаются по сети с использованием сокетов. Далее серверы принимают информацию о булавках от клиентов и выводят сообщения не экран.

Код клиента:

```

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    // Создание сокета клиента
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Ошибка при создании сокета клиента");
        exit(1);
    }

    // Настройка адреса сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(PORT);

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Ошибка при подключении к серверу");
        exit(1);
    }

    printf("Подключение рабочего к серверу установлено\n");

    // Отправка данных серверу
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

    // Закрытие сокета клиента
    close(client_socket);

    return 0;
}
```

Код сервера: 
```
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
```

#Код на 6-7:

Аналогично предыдущим пунктам, только добавляется наблюдатель. Теперь сервер отправляет сообщения и рабочим, и наблюдателю. 

Код клиента:
```
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
```

Код сервера:
```
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
        perror("Создание сокета не произошло");
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

    update_system_status("Сервер запущен");

    if ((observer_socket = accept(server_fd, (struct sockaddr *)&observer_address, (socklen_t*)&addrlen)) < 0) {
        perror("Наблюдатель не смог подключиться");
        exit(EXIT_FAILURE);
    }

    update_system_status("Надлюдатель подключен");

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Ошибка подключения");
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
        snprintf(status_message, sizeof(status_message), "Клиент подключен: %d", new_socket);
        send(observer_socket, status_message, strlen(status_message), 0);

        close(new_socket);
    }

    close(observer_socket);
    close(server_fd);

    return 0;
}
```

Код наблюдателя: 
```

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void display_system_status(const char *status) {
    printf("Наблюдатель: %s\n", status);
}

int main() {
    int observer_socket;
    struct sockaddr_in server_address;

    if ((observer_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Сокет не создан");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &(server_address.sin_addr)) <= 0) {
        perror("Неверный адрес");
        exit(EXIT_FAILURE);
    }

    if (connect(observer_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Соединение не установлено");
        exit(EXIT_FAILURE);
    }

    char status_message[100];
    while (1) {
        memset(status_message, 0, sizeof(status_message));

        // Получение информации от сервера
        if (recv(observer_socket, status_message, sizeof(status_message), 0) <= 0) {
            perror("Получение информации от сервера провалено");
            exit(EXIT_FAILURE);
        }

        display_system_status(status_message);
    }

    close(observer_socket);

    return 0;
}
```



