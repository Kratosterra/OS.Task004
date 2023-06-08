#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../functions/function.c"
#include <signal.h>

int client_fd;

void handler(int signum)
{
    printf("Получен сигнал, завершаем работу!\n");
    close(client_fd);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <ip адрес> <порт>\n", argv[0]);
        return 1;
    }
    signal(SIGINT, handler);
    char *ip = argv[1];
    int port = atoi(argv[2]);

    int all_op = 0;
    struct sockaddr_in serv_addr;
    double subrange_start, subrange_end, subrange_result;

    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Ошибка при создании сокета клиента.");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &(serv_addr.sin_addr)) <= 0)
    {
        perror("Ошибка при конвертации ip");
        exit(EXIT_FAILURE);
    }

    printf("Пытаемся отправить запрос серверу...\n");
    sendto(client_fd, NULL, 0, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    double a, b;
    int all_clients;
    printf("Наблюдатель подключился. Ожидаем обновлений...\n");
    recvfrom(client_fd, &a, sizeof(double), 0, NULL, NULL);
    recvfrom(client_fd, &b, sizeof(double), 0, NULL, NULL);
    recvfrom(client_fd, &all_clients, sizeof(int), 0, NULL, NULL);
    printf("Общая информация.\na = %f\nb = %f\nКол-во клиентов: %d\n\n", a, b, all_clients);
    while (1)
    {
        int valread = 0;
        int num_clients;
        double subrange_start, subrange_end, client_result, total_area;
        valread = recvfrom(client_fd, &subrange_start, sizeof(double), 0, NULL, NULL);
        if (valread == 0)
        {
            printf("Сервер отключён.\n");
            break;
        }
        recvfrom(client_fd, &subrange_end, sizeof(double), 0, NULL, NULL);
        recvfrom(client_fd, &num_clients, sizeof(int), 0, NULL, NULL);
        recvfrom(client_fd, &client_result, sizeof(double), 0, NULL, NULL);
        recvfrom(client_fd, &total_area, sizeof(double), 0, NULL, NULL);
        printf("Клиент №%d\n", all_clients - num_clients + 1);
        printf("Область [%f, %f]\nКлиент выдал результат: %f\nОбщая площадь: %f\n\n",
               subrange_start, subrange_end, client_result, total_area);
    }
    close(client_fd);
    return 0;
}