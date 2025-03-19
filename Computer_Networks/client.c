#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int errno;

int port;

void menu()
{
    printf("\nComenzile disponibile sunt:\n");
    printf("1. create_account\n");
    printf("2. login\n");
    printf("3. view_products\n");
    printf("4. search_product\n");
    printf("5. sort_products\n");
    printf("6. view_price\n");
    printf("7. buy_product\n");
    printf("8. view_history\n");
    printf("9. sell_product\n");
    printf("10. logout\n");
}

void handle_create_account(int sd)
{
    char ans[100];
    char username[100], password[100];
    int bytesRead = 0;

    if (write(sd, "create_account", 14) <= 0)
    {
        perror("[client]Eroare la trimiterea comenzii create_account.\n");
        return;
    }

    bzero(ans, 100);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(username, 100);
    read(0, username, 100);
    username[strcspn(username, "\n")] = '\0';
    if (write(sd, username, strlen(username)) <= 0)
    {
        perror("[client]Eroare la trimiterea numelui de utilizator.\n");
        return;
    }

    bzero(ans, 100);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(password, 100);
    read(0, password, 100);
    password[strcspn(password, "\n")] = '\0';
    if (write(sd, password, strlen(password)) <= 0)
    {
        perror("[client]Eroare la trimiterea parolei.\n");
        return;
    }

    char response[150];
    bzero(response, 150);
    if (read(sd, response, 150) > 0)
    {
        printf("[client] Mesajul primit de la server: %s\n", response);
    }
    else
    {
        perror("[client]Eroare la primirea răspunsului de la server.\n");
    }
}

void handle_login(int sd)
{
    char ans[100];
    char username[100], password[100];
    int bytesRead = 0;

    if (write(sd, "login", 5) <= 0)
    {
        perror("[client]Eroare la trimiterea comenzii login.\n");
        return;
    }

    bzero(ans, 100);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(username, 100);
    read(0, username, 100);
    username[strcspn(username, "\n")] = '\0';
    if (write(sd, username, strlen(username)) <= 0)
    {
        perror("[client]Eroare la trimiterea username-ului.\n");
        return;
    }

    bzero(ans, 100);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }
    bzero(password, 100);
    read(0, password, 100);
    password[strcspn(password, "\n")] = '\0';
    if (write(sd, password, strlen(password)) <= 0)
    {
        perror("[client]Eroare la trimiterea parolei.\n");
        return;
    }

    char response[150];
    bzero(response, 150);
    if (read(sd, response, 150) > 0)
    {
        printf("[client] Mesajul primit de la server: %s\n", response);
    }
    else
    {
        perror("[client]Eroare la primirea răspunsului de la server.\n");
    }
}

void handle_sell_product(int sd)
{
    char product_name[100], price[100];
    char ans[100];
    int bytesRead = 0;

    if (write(sd, "sell_product", 12) <= 0)
    {
        perror("[client]Eroare la trimiterea comenzii sell_product.\n");
        return;
    }

    bzero(ans, 100);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(product_name, 100);
    read(0, product_name, 100);
    product_name[strcspn(product_name, "\n")] = '\0';
    if (write(sd, product_name, strlen(product_name)) <= 0)
    {
        perror("[client]Eroare la trimiterea numelui produsului.\n");
        return;
    }

    bzero(ans, 100);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(price, 100);
    read(0, price, 100);
    price[strcspn(price, "\n")] = '\0';
    if (write(sd, price, strlen(price)) <= 0)
    {
        perror("[client]Eroare la trimiterea pretului produsului.\n");
        return;
    }

    char response[150];
    bzero(response, 150);
    if (read(sd, response, 150) > 0)
    {
        printf("[client] Mesajul primit de la server: %s\n", response);
    }
    else
    {
        perror("[client]Eroare la primirea răspunsului de la server.\n");
    }
}

void handle_search_product(int sd)
{
    char product_name[100];
    char ans[1024];
    int bytesRead = 0;

    if (write(sd, "search_product", 14) <= 0)
    {
        perror("[client]Eroare la trimiterea comenzii search_product.\n");
        return;
    }

    bzero(ans, 1024);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(product_name, 100);
    read(0, product_name, 100);
    product_name[strcspn(product_name, "\n")] = '\0';

    if (write(sd, product_name, strlen(product_name)) <= 0)
    {
        perror("[client]Eroare la trimiterea numelui produsului.\n");
        return;
    }

    char response[1024];
    bzero(response, 1024);
    if (read(sd, response, 1024) > 0)
    {
        printf("[client] Mesajul primit de la server: %s\n", response);
    }
    else
    {
        perror("[client]Eroare la primirea răspunsului de la server.\n");
    }
}

void handle_view_price(int sd)
{
    char product_id[100];
    char ans[1024];
    int bytesRead = 0;

    if (write(sd, "view_price", 10) <= 0)
    {
        perror("[client]Eroare la trimiterea comenzii view_price.\n");
        return;
    }

    bzero(ans, 1024);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(product_id, 100);
    read(0, product_id, 100);
    product_id[strcspn(product_id, "\n")] = '\0';

    if (write(sd, product_id, strlen(product_id)) <= 0)
    {
        perror("[client]Eroare la trimiterea numelui produsului.\n");
        return;
    }

    char response[1024];
    bzero(response, 1024);
    if (read(sd, response, 1024) > 0)
    {
        printf("[client] Mesajul primit de la server: \n- %s\n", response);
    }
    else
    {
        perror("[client]Eroare la primirea răspunsului de la server.\n");
    }
}

void handle_buy_product(int sd)
{
    char product_id[100];
    char ans[1024];
    int bytesRead = 0;

    if (write(sd, "buy_product", 11) <= 0)
    {
        perror("[client]Eroare la trimiterea comenzii buy_product.\n");
        return;
    }

    bzero(ans, 1024);
    while ((bytesRead = read(sd, ans, sizeof(ans) - 1)) > 0)
    {
        ans[bytesRead] = '\0';
        printf("[client] %s\n", ans);

        if (strncmp(ans, "Eroare", 6) == 0)
        {
            return;
        }

        if (bytesRead < sizeof(ans) - 1)
        {
            break;
        }
    }

    if (bytesRead < 0)
    {
        perror("[client]Eroare la read() de la server.\n");
        return;
    }

    bzero(product_id, 100);
    read(0, product_id, 100);
    product_id[strcspn(product_id, "\n")] = '\0';

    if (write(sd, product_id, strlen(product_id)) <= 0)
    {
        perror("[client]Eroare la trimiterea id-ului produsului.\n");
        return;
    }

    char response[150];
    bzero(response, 150);
    if (read(sd, response, 150) > 0)
    {
        printf("[client] Mesajul primit de la server: \n- %s\n", response);
    }
    else
    {
        perror("[client]Eroare la primirea răspunsului de la server.\n");
    }
}

int main(int argc, char *argv[])
{
    int sd;
    struct sockaddr_in server;
    char command[100];

    if (argc != 3)
    {
        printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[client]Eroare la connect().\n");
        return errno;
    }
    menu();

    while (1)
    {
        int bytesRead = 0;
        bzero(command, 100);
        printf("[client] Introduceti comanda: ");
        fflush(stdout);
        read(0, command, 100);
        command[strcspn(command, "\n")] = '\0';

        if (strncmp(command, "logout", 6) == 0)
        {
            printf("[client] Deconectare...\n");
            break;
        }

        if (strncmp(command, "login", 5) == 0)
        {
            handle_login(sd);
            continue;
        }

        if (strncmp(command, "create_account", 14) == 0)
        {
            handle_create_account(sd);
            continue;
        }

        if (strncmp(command, "sell_product", 12) == 0)
        {
            handle_sell_product(sd);
            continue;
        }

        if (strncmp(command, "search_product", 14) == 0)
        {
            handle_search_product(sd);
            continue;
        }

        if (strncmp(command, "view_price", 10) == 0)
        {
            handle_view_price(sd);
            continue;
        }

        if (strncmp(command, "buy_product", 11) == 0)
        {
            handle_buy_product(sd);
            continue;
        }

        // pentru restul comenzilor
        if (write(sd, command, strlen(command)) <= 0)
        {
            perror("[client]Eroare la write() spre server.\n");
            return errno;
        }

        char command_ans[1024];
        bzero(command_ans, 1024);

        while ((bytesRead = read(sd, command_ans, sizeof(command_ans) - 1)) > 0)
        {
            command_ans[bytesRead] = '\0';
            printf("[client] %s\n", command_ans);

            if (bytesRead < sizeof(command_ans) - 1)
            {
                break;
            }
        }

        if (bytesRead < 0)
        {
            perror("[client]Eroare la read() de la server.\n");
            return errno;
        }

        if (strncmp(command_ans, "Comandă necunoscută", 19) == 0)
        {
            menu();
        }
    }

    close(sd);
}