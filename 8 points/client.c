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
    int info = 1;
    printf("Пытаемся отправить запрос серверу...\n");
    sendto(client_fd, NULL, 0, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    sendto(client_fd, &info, sizeof(int), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    recvfrom(client_fd, &subrange_start, sizeof(double), 0, NULL, NULL);
    recvfrom(client_fd, &subrange_end, sizeof(double), 0, NULL, NULL);
    recvfrom(client_fd, &all_op, sizeof(int), 0, NULL, NULL);

    printf("Счетовод получил задание. Область расчёта: [%f, %f]\n", subrange_start, subrange_end);
    subrange_result = calculateArea(subrange_start, subrange_end, all_op);
    printf("Счетовод выполнил задание. Подсчитал: %f кв.м.\n", subrange_result);

    printf("Предоставляем результат агроному...\n");
    sendto(client_fd, &subrange_result, sizeof(double), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Счетовод идёт домой отдыхать!\n");
    close(client_fd);
    return 0;
}