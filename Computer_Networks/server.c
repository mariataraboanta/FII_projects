#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <sys/wait.h>

#define PORT 2024

extern int errno;

void database(sqlite3 *db)
{
    char *err_msg = 0;

    const char *create_clients_table =
        "CREATE TABLE IF NOT EXISTS Clients ("
        "client_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nume TEXT NOT NULL UNIQUE, "
        "parola TEXT NOT NULL);";

    const char *create_products_table =
        "CREATE TABLE IF NOT EXISTS Products ("
        "product_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nume TEXT NOT NULL, "
        "pret REAL NOT NULL);";

    const char *create_transactions_table =
        "CREATE TABLE IF NOT EXISTS Transactions ("
        "transaction_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "client_id INTEGER NOT NULL, "
        "product_id INTEGER NOT NULL, "
        "nume_produs TEXT NOT NULL, "
        "pret_total REAL NOT NULL, "
        "data_tranzactiei TEXT DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY(client_id) REFERENCES Clients(client_id), "
        "FOREIGN KEY(product_id) REFERENCES Products(product_id));";

    if (sqlite3_exec(db, create_clients_table, 0, 0, &err_msg) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la crearea tabelului Clients: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    if (sqlite3_exec(db, create_products_table, 0, 0, &err_msg) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la crearea tabelului Products: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    if (sqlite3_exec(db, create_transactions_table, 0, 0, &err_msg) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la crearea tabelului Transactions: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    printf("Baza de date a fost inițializată.\n");
}

void handle_create_account(int client, sqlite3 *db, int *is_logged_in)
{
    if (*is_logged_in)
    {
        write(client, "Eroare: Deconectati-va pentru a crea un cont nou.", 50);
        return;
    }

    char username[100], password[100], message[100];
    bzero(username, 100);
    bzero(password, 100);

    bzero(message, 100);
    strcat(message, "Introduceti username: ");
    printf("[server]Trimitem mesajul inapoi...%s\n", message);
    write(client, message, strlen(message));

    if (read(client, username, sizeof(username)) <= 0)
    {
        perror("[server]Eroare la citirea numelui de utilizator.\n");
        close(client);
        return;
    }
    else
    {
        username[strcspn(username, "\n")] = '\0';
        printf("[server]Numele de utilizator primit: %s\n", username);
    }

    bzero(message, 100);
    strcat(message, "Introduceti parola: ");
    printf("[server]Trimitem mesajul inapoi...%s\n", message);
    write(client, message, strlen(message));

    if (read(client, password, sizeof(password)) <= 0)
    {
        perror("[server]Eroare la citirea parolei.\n");
        close(client);
        return;
    }
    else
    {
        password[strcspn(password, "\n")] = '\0'; 
        printf("[server]Parola primită: %s\n", password);
    }

    // inseram in baza de date
    char *err_msg = 0;
    char sql[256];
    snprintf(sql, 256, "INSERT INTO Clients (nume, parola) VALUES ('%s', '%s');", username, password);

    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK)
    {
        if (strstr(err_msg, "UNIQUE"))
        {
            write(client, "Eroare: Numele de utilizator exista deja.", 42);
        }
        else
        {
            write(client, "Eroare: Nu s-a putut crea contul.", 34);
        }
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    else
    {
        write(client, "Cont creat cu succes.", 22);
    }
}

void handle_login(int client, sqlite3 *db, int *is_logged_in, int *logged_in_user_id)
{
    char username[100], password[100], message[100];
    bzero(username, 100);
    bzero(password, 100);

    if (*is_logged_in)
    {
        write(client, "Eroare: Utilizator deja logat.", 31);
        return;
    }

    bzero(message, 100);
    strcat(message, "Introduceti username: ");
    printf("[server]Trimitem mesajul inapoi...%s\n", message);
    write(client, message, strlen(message));

    if (read(client, username, sizeof(username)) <= 0)
    {
        perror("[server]Eroare la citirea numelui de utilizator.\n");
        close(client);
        return;
    }
    else
    {
        username[strcspn(username, "\n")] = '\0';
        printf("[server] Numele de utilizator primit: %s\n", username);
    }

    // verificam daca username-ul exista
    sqlite3_stmt *stmt;
    const char *sql = "SELECT client_id, parola FROM Clients WHERE nume = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare server.", 15);
        return;
    }

    if (sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la legarea parametrului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare server.", 15);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        write(client, "Eroare: Username inexistent.", 29);
        sqlite3_finalize(stmt);
        return;
    }

    int user_id = sqlite3_column_int(stmt, 0);
    const char *correct_password = (const char *)sqlite3_column_text(stmt, 1);
    char db_password[100];
    strncpy(db_password, correct_password, sizeof(db_password) - 1);
    db_password[sizeof(db_password) - 1] = '\0';
    sqlite3_finalize(stmt);

    bzero(message, 100);
    strcat(message, "Introduceti parola: ");
    printf("[server]Trimitem mesajul inapoi...%s\n", message);
    write(client, message, strlen(message));

    if (read(client, password, sizeof(password)) <= 0)
    {
        perror("[server]Eroare la citirea parolei.\n");
        close(client);
        return;
    }
    else
    {
        password[strcspn(password, "\n")] = '\0';
        printf("[server] Parola primită: %s\n", password);
    }

    // verificam parola
    if (strcmp(password, db_password) == 0)
    {
        *is_logged_in = 1;
        *logged_in_user_id = user_id;
        write(client, "Autentificare reusita.", 23);
    }
    else
    {
        write(client, "Eroare: Parola incorecta.", 26);
    }
}

void handle_sell_product(int client, sqlite3 *db)
{
    char product_name[100], message[100];
    double price;
    bzero(product_name, 100);
    bzero(message, 100);

    strcat(message, "Introduceti numele produsului: ");
    write(client, message, strlen(message));
    if (read(client, product_name, sizeof(product_name)) <= 0)
    {
        perror("[server]Eroare la citirea numelui produsului.\n");
        close(client);
        return;
    }
    product_name[strcspn(product_name, "\n")] = '\0';
    printf("[server]Numele produsului primit: %s\n", product_name);

    bzero(message, 100);
    strcat(message, "Introduceti pretul produsului: ");
    write(client, message, strlen(message));

    char price_str[100];
    bzero(price_str, 100);
    if (read(client, price_str, sizeof(price_str)) <= 0)
    {
        perror("[server]Eroare la citirea pretului produsului.\n");
        close(client);
        return;
    }
    price = atof(price_str);
    if (price == 0.00)
    {
        write(client, "Eroare: pret invalid.", 22);
        return;
    }
    printf("[server]Pretul produsului primit: %.2f\n", price);

    // inseram in tabela products
    char *err_msg = 0;
    char sql[256];
    snprintf(sql, 256, "INSERT INTO Products (nume, pret) VALUES ('%s', %.2f);", product_name, price);

    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK)
    {
        write(client, "Eroare: Nu s-a putut adauga produsul.", 38);
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    else
    {
        write(client, "Produs adaugat cu succes.", 26);
    }
}

void handle_view_products(int client, sqlite3 *db)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT product_id, nume FROM Products;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    char message[1024];
    bzero(message, 1024);
    strcat(message, "Lista produselor:\n");

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int product_id = sqlite3_column_int(stmt, 0);
        const char *product_name = (const char *)sqlite3_column_text(stmt, 1);

        strcat(message, "- id: ");
        char id_str[20];
        snprintf(id_str, sizeof(id_str), "%d", product_id);
        strcat(message, id_str);
        strcat(message, " - ");
        strcat(message, product_name);
        strcat(message, "\n");
    }

    sqlite3_finalize(stmt);

    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea listei produselor.\n");
    }
}

void handle_sort_products(int client, sqlite3 *db)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT product_id, nume FROM Products ORDER BY nume ASC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    char message[1024];
    bzero(message, 1024);
    strcat(message, "Produsele sortate alfabetic:\n");

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int product_id = sqlite3_column_int(stmt, 0);
        const char *product_name = (const char *)sqlite3_column_text(stmt, 1);

        strcat(message, "- id: ");
        char id_str[20];
        snprintf(id_str, sizeof(id_str), "%d", product_id);
        strcat(message, id_str);
        strcat(message, " - ");
        strcat(message, product_name);
        strcat(message, "\n");
    }

    sqlite3_finalize(stmt);

    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea listei produselor sortate.\n");
    }
}

void handle_search_product(int client, sqlite3 *db)
{
    char product_name[100];
    char message[1024];

    bzero(message, 1024);
    strcat(message, "Introduceti numele produsului de cautat: ");
    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea solicitarii pentru numele produsului.\n");
        return;
    }

    bzero(product_name, 100);
    if (read(client, product_name, sizeof(product_name)) <= 0)
    {
        perror("[server]Eroare la citirea numelui produsului.\n");
        close(client);
        return;
    }
    product_name[strcspn(product_name, "\n")] = '\0';
    printf("[server]Numele produsului primit: %s\n", product_name);

    // cautam produsele cu numele primit
    sqlite3_stmt *stmt;
    const char *sql = "SELECT product_id, nume FROM Products WHERE nume = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    if (sqlite3_bind_text(stmt, 1, product_name, -1, SQLITE_STATIC) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la legarea parametrului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    bzero(message, 1024);
    strcat(message, "Rezultatele cautarii:\n");

    int found = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;
        int product_id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);

        char result[256];
        snprintf(result, sizeof(result), "- id: %d - %s\n", product_id, name);
        strcat(message, result);
    }

    sqlite3_finalize(stmt);

    if (!found)
    {
        strcat(message, "Eroare: Niciun produs gasit.\n");
    }

    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea rezultatelor cautarii.\n");
    }
}

void handle_view_price(int client, sqlite3 *db)
{
    char product_id_str[10];
    char message[1024];

    bzero(message, 1024);
    strcat(message, "Introduceti id-ul produsului: ");
    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea solicitarii pentru id-ul produsului.\n");
        return;
    }

    bzero(product_id_str, 10);
    if (read(client, product_id_str, sizeof(product_id_str)) <= 0)
    {
        perror("[server]Eroare la citirea id-ului produsului.\n");
        close(client);
        return;
    }
    int product_id = atoi(product_id_str);

    // interogam baza de date pentru produsul cu id-ul primit
    sqlite3_stmt *stmt;
    const char *sql = "SELECT product_id, nume, pret FROM Products WHERE product_id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    if (sqlite3_bind_int(stmt, 1, product_id) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la legarea parametrului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    bzero(message, 1024);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        double price = sqlite3_column_double(stmt, 2);

        snprintf(message, sizeof(message), "id: %d - nume: %s - pret: %.2f\n", id, name, price);
    }
    else
    {
        strcat(message, "Eroare: Produsul cu id-ul specificat nu exista.\n");
    }

    sqlite3_finalize(stmt);

    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea informatiilor despre produs.\n");
    }
}

void handle_buy_product(int client, sqlite3 *db, int logged_in_user_id)
{
    char product_id_str[10];
    char message[1024];

    bzero(message, 1024);
    strcat(message, "Introduceti id-ul produsului pe care doriti sa il cumparati: ");
    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea solicitarii pentru ID-ul produsului.\n");
        return;
    }

    bzero(product_id_str, 10);
    if (read(client, product_id_str, sizeof(product_id_str)) <= 0)
    {
        perror("[server]Eroare la citirea id-ului produsului.\n");
        close(client);
        return;
    }
    int product_id = atoi(product_id_str);

    // verificam daca produsul exista si extragem detaliile produsului
    sqlite3_stmt *stmt;
    const char *select_sql = "SELECT product_id, nume, pret FROM Products WHERE product_id = ?;";
    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    if (sqlite3_bind_int(stmt, 1, product_id) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la legarea parametrului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        write(client, "Produsul cu id-ul specificat nu exista.", 40);
        return;
    }

    const unsigned char *raw_product_name = sqlite3_column_text(stmt, 1);
    char product_name[256];
    strncpy(product_name, (const char *)raw_product_name, sizeof(product_name) - 1);
    product_name[sizeof(product_name) - 1] = '\0';
    double product_price = sqlite3_column_double(stmt, 2);
    sqlite3_finalize(stmt);

    // inregistram tranzactia in tabela transactions
    const char *insert_sql = "INSERT INTO Transactions (client_id, product_id, nume_produs, pret_total) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la inregistrarea tranzactiei.", 37);
        return;
    }

    if (sqlite3_bind_int(stmt, 1, logged_in_user_id) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, product_id) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, product_name, -1, SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_double(stmt, 4, product_price) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la legarea parametrului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la inregistrarea tranzactiei.", 37);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr, "Eroare la inregistrarea tranzactiei: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la inregistrarea tranzactiei.", 37);
        return;
    }
    sqlite3_finalize(stmt);

    // stergem produsul din tabela products
    const char *delete_sql = "DELETE FROM Products WHERE product_id = ?;";
    if (sqlite3_prepare_v2(db, delete_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la stergerea produsului.", 32);
        return;
    }

    if (sqlite3_bind_int(stmt, 1, product_id) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la legarea parametrului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la stergerea produsului.", 32);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        fprintf(stderr, "Eroare la stergerea produsului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la stergerea produsului.", 32);
        return;
    }
    sqlite3_finalize(stmt);

    write(client, "Produsul a fost cumparat cu succes.", 36);
}

void handle_view_history(int client, sqlite3 *db, int logged_in_user_id)
{
    char message[1024];
    sqlite3_stmt *stmt;

    if (logged_in_user_id <= 0)
    {
        write(client, "Eroare: Trebuie sa fiti autentificat pentru a vizualiza istoricul tranzactiilor.", 78);
        return;
    }

    const char *sql = "SELECT Transactions.transaction_id, Transactions.product_id, Transactions.nume_produs, Transactions.pret_total, Transactions.data_tranzactiei "
                      "FROM Transactions "
                      "WHERE Transactions.client_id = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la interogarea bazei de date: %s\n", sqlite3_errmsg(db));
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    if (sqlite3_bind_int(stmt, 1, logged_in_user_id) != SQLITE_OK)
    {
        fprintf(stderr, "Eroare la legarea parametrului: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        write(client, "Eroare la interogarea bazei de date.", 37);
        return;
    }

    bzero(message, 1024);
    strcat(message, "Istoricul tranzactiilor:\n");

    int found = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = 1;
        int transaction_id = sqlite3_column_int(stmt, 0);
        int product_id = sqlite3_column_int(stmt, 1);
        const char *product_name = (const char *)sqlite3_column_text(stmt, 2);
        double total_price = sqlite3_column_double(stmt, 3);
        const char *transaction_date = (const char *)sqlite3_column_text(stmt, 4);

        char result[256];
        snprintf(result, sizeof(result), "- id tranzactie: %d\n- id produs: %d\n- nume produs: %s\n- pret: %.2f\n- data: %s\n",
                 transaction_id, product_id, product_name, total_price, transaction_date);
        strcat(message, result);
    }

    sqlite3_finalize(stmt);

    if (!found)
    {
        strcat(message, "Nu exista tranzactii inregistrate pentru acest utilizator.\n");
    }

    if (write(client, message, strlen(message)) <= 0)
    {
        perror("[server]Eroare la trimiterea istoricului tranzactiilor.\n");
    }
}

void handle_command(int client, sqlite3 *db)
{
    char command[100];
    int is_logged_in = 0;
    int logged_in_user_id = -1;
    while (1)
    {
        bzero(command, 100);
        printf("[server]Asteptam mesajul...\n");
        fflush(stdout);

        if (read(client, command, sizeof(command)) <= 0)
        {
            perror("[server]Eroare la read() de la client.\n");
            close(client);
            return;
        }
        command[strcspn(command, "\n")] = '\0';
        printf("[server]Comanda primita este: %s\n", command);

        char command_ans[150];
        bzero(command_ans, 150);
        strcat(command_ans, "Comanda recunoscuta: ");
        strcat(command_ans, command);

        printf("[server]Trimitem mesajul inapoi...%s\n", command_ans);

        if (strncmp(command, "create_account", 14) == 0)
        {
            handle_create_account(client, db, &is_logged_in);
        }
        else if (strncmp(command, "logout", 6) == 0)
        {
            is_logged_in = 0;
            logged_in_user_id = -1;
            write(client, command_ans, strlen(command_ans));
            break; // iesim din bucla cand se primeste logout
        }
        else if (strncmp(command, "login", 5) == 0)
        {
            handle_login(client, db, &is_logged_in, &logged_in_user_id);
        }
        else if (is_logged_in)
        {
            if (strncmp(command, "view_products", 13) == 0)
            {
                handle_view_products(client, db);
            }
            else if (strncmp(command, "search_product", 14) == 0)
            {
                handle_search_product(client, db);
            }
            else if (strncmp(command, "sort_products", 13) == 0)
            {
                handle_sort_products(client, db);
            }
            else if (strncmp(command, "view_price", 10) == 0)
            {
                handle_view_price(client, db);
            }
            else if (strncmp(command, "buy_product", 11) == 0)
            {
                handle_buy_product(client, db, logged_in_user_id);
            }
            else if (strncmp(command, "view_history", 12) == 0)
            {
                handle_view_history(client, db, logged_in_user_id);
            }
            else if (strncmp(command, "sell_product", 12) == 0)
            {
                handle_sell_product(client, db);
            }
            else
            {
                write(client, "Comandă necunoscută", 22);
            }
        }
        else
        {
            write(client, "Eroare: Niciun user logat.", 27);
        }
    }
    close(client);
}

int main()
{
    struct sockaddr_in server;
    struct sockaddr_in from;
    int sd;
    sqlite3 *db;

    if (sqlite3_open("localmarketplace.db", &db))
    {
        fprintf(stderr, "Eroare la deschiderea bazei de date: %s\n", sqlite3_errmsg(db));
        return 1;
    }
    database(db);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server]Eroare la socket().\n");
        return errno;
    }

    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("[server]Eroare la bind().\n");
        sqlite3_close(db);
        return errno;
    }

    if (listen(sd, 1) == -1)
    {
        perror("[server]Eroare la listen().\n");
        sqlite3_close(db);
        return errno;
    }

    while (1)
    {
        int client;
        int length = sizeof(from);

        printf("[server]Asteptam la portul %d...\n", PORT);
        fflush(stdout);

        client = accept(sd, (struct sockaddr *)&from, &length);

        if (client < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        int pid;
        if ((pid = fork()) == -1)
        {
            close(client);
            continue;
        }
        else if (pid > 0)
        {
            // parinte
            close(client);
            while (waitpid(-1, NULL, WNOHANG))
                ;
            continue;
        }
        else if (pid == 0)
        {
            // copil
            close(sd);
            handle_command(client, db);
            exit(0);
        }
    }
    sqlite3_close(db);
}