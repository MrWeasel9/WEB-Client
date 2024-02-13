#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include "helpers.h"
#include "requests.h"
#include "parson.c"

#define HOST "34.254.242.81"
#define PORT 8080
#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define LOGOUT "/api/v1/tema/auth/logout"
#define ENTER "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"

bool isInteger(const char* id) {
    if (strlen(id) == 0)
        return false;
    for (int i = 0; i < strlen(id); i++) {
        if (!isdigit(id[i]))
            return false;
    }
    return true;
}
int main(int argc, char* argv[])
{
    char* message;
    char* response;

    char* cookies = NULL;
    char* token = NULL;

    char username[BUFLEN];
    char password[BUFLEN];

    char input[LINELEN];
    int connected = 0;
    int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);


    while (true) {

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (strcmp(input, "exit") == 0) {
            break;
        }

        if (strcmp(input, "register") == 0) {

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            printf("username=");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = '\0';

            printf("password=");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = '\0';

            JSON_Value* rootValue = json_value_init_object();
            JSON_Object* rootObject = json_value_get_object(rootValue);

            json_object_set_string(rootObject, "username", username);
            json_object_set_string(rootObject, "password", password);

            char* payload = json_serialize_to_string(rootValue);

            // Fac cererea de tip POST
            message = compute_post_request(HOST, REGISTER, "application/json", payload, NULL, NULL);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            char* error = basic_extract_json_response(response);

            if (error == NULL) {
                printf("You have successfully registered.\n");
            }
            else {
                printf("%s\n", error);
            }
            // Eliberare memorie
            json_free_serialized_string(payload);
            json_value_free(rootValue);
        }
        else if (strcmp(input, "login") == 0) {

            if (connected == 1) {
                printf("You are already logged in!\n");
                continue;
            }
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            printf("username=");
            fgets(username, sizeof(username), stdin);
            username[strcspn(username, "\n")] = '\0';

            printf("password=");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = '\0';

            JSON_Value* rootValue = json_value_init_object();
            JSON_Object* rootObject = json_value_get_object(rootValue);

            json_object_set_string(rootObject, "username", username);
            json_object_set_string(rootObject, "password", password);

            char* payload = json_serialize_to_string(rootValue);

            message = compute_post_request(HOST, LOGIN, "application/json", payload, NULL, NULL);
            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            char* error = basic_extract_json_response(response);

            if (error == NULL) {
                printf("You have successfully logged in.\n");

                // Cookie
                char* start = strstr(response, "Set-Cookie:");
                char* p = strtok(start + 12, ";");
                cookies = malloc(strlen(p) + 1);
                memcpy(cookies, p, strlen(p));
                connected = 1;
            }
            else {
                // Printez eroarea
                printf("%s\n", error);
            }

            // Eliberez memoria
            json_free_serialized_string(payload);
            json_value_free(rootValue);
        }
        else if (strcmp(input, "enter_library") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(HOST, ENTER, NULL, cookies, NULL);

            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            char* responseValue = basic_extract_json_response(response);

            if (strstr(responseValue, "error") == NULL) {
                // Daca mesajul nu contine un string json eroare,
                // atunci intrarea in bibilioteca a fost facuta cu succes
                printf("You have entered the library.\n");

                // Extrag token-ul din mesajul primit
                char* p = strtok(responseValue + 10, "\"");
                token = malloc(strlen(p) + 1);
                memcpy(token, p, strlen(p));
            }
            else {
                // Printez eroarea
                printf("%s\n", responseValue);

            }
        }
        else if (strcmp(input, "get_books") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(HOST, BOOKS, NULL, cookies, token);

            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            char* responseValue = basic_extract_json_response(response);

            if (responseValue == NULL) {
                printf("There are no books!\n");
            }
            else {
                printf("%s\n", responseValue);
            }
        }
        else if (strcmp(input, "get_book") == 0) {

            printf("id=");
            scanf("%s", input);


            if (!isInteger(input)) {
                printf("Id invalid format.\n");
            }
            else {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                char url[BUFLEN];
                int id = atoi(input);
                sprintf(url, "/api/v1/tema/library/books/%d", id);

                message = compute_get_request(HOST, url, NULL, cookies, token);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                char* responseValue = basic_extract_json_response(response);

                printf("%s\n", responseValue);
            }
        }
        else if (strcmp(input, "add_book") == 0) {

            char title[LINELEN];
            char author[LINELEN];
            char genre[LINELEN];
            char page_count_string[LINELEN];
            int page_count;
            char publisher[LINELEN];

            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            // Citire titlu, autor, gen, publisher, page_count
            printf("title=");
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = '\0';

            printf("author=");
            fgets(author, sizeof(author), stdin);
            author[strcspn(author, "\n")] = '\0';

            printf("genre=");
            fgets(genre, sizeof(genre), stdin);
            genre[strcspn(genre, "\n")] = '\0';

            printf("publisher=");
            fgets(publisher, sizeof(publisher), stdin);
            publisher[strcspn(publisher, "\n")] = '\0';

            printf("page_count=");
            fgets(page_count_string, sizeof(page_count_string), stdin);
            page_count_string[strcspn(page_count_string, "\n")] = '\0';
            page_count = atoi(page_count_string);

            JSON_Value* rootValue = json_value_init_object();
            JSON_Object* rootObject = json_value_get_object(rootValue);

            json_object_set_string(rootObject, "title", title);
            json_object_set_string(rootObject, "author", author);
            json_object_set_string(rootObject, "genre", genre);

            if (isInteger(page_count_string)) {
                json_object_set_number(rootObject, "page_count", page_count);
            }
            else {
                json_object_set_string(rootObject, "page_count", page_count_string);
            }

            json_object_set_string(rootObject, "publisher", publisher);
            char* serialized_string = json_serialize_to_string(rootValue);

            message = compute_post_request(HOST, BOOKS, "application/json", serialized_string, cookies, token);

            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            char* responseValue = basic_extract_json_response(response);

            if (responseValue != NULL) {
                if (strstr(responseValue, "error") != NULL) {
                    // Printez eroarea
                    printf("%s\n", responseValue);
                }
            }
            else {
                if (strstr(response, "Too many requests") != NULL) {
                    printf("Too many requests, please try again later.\n");
                }
                else {
                    printf("You have successfully added the book.\n");
                }
            }
        }
        else if (strcmp(input, "delete_book") == 0) {

            printf("id=");
            scanf("%s", input);

            if (!isInteger(input)) {
                printf("Id invalid format.\n");
            }
            else {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

                char str[BUFLEN];
                int id = atoi(input);
                sprintf(str, "/api/v1/tema/library/books/%d", id);

                message = compute_delete_request(HOST, str, NULL, cookies, token);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                char* responseValue = basic_extract_json_response(response);

                if (responseValue != NULL) {
                    // Printez eroarea
                    if (strstr(responseValue, "error") != NULL) {
                       printf("%s\n", responseValue);
                    }
                }
                else {
                    if (strstr(response, "Too many requests") != NULL) {
                        printf("Too many requests, please try again later.\n");
                    }
                    else {
                        printf("You have successfully deleted the book.\n");
                    }
                }
            }
        }
        else if (strcmp(input, "logout") == 0) {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

            message = compute_get_request(HOST, LOGOUT, NULL, cookies, token);

            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            char* responseValue = basic_extract_json_response(response);

            if (responseValue != NULL) {
                // Printez eroarea
                if (strstr(responseValue, "error") != NULL) {
                    printf("%s\n", responseValue);
                }
            }
            else {
                printf("You have logged out.\n");
                // Sterg tokenul si cookieurile si schimb semaforul connected
                connected = 0;
                token = NULL;
                cookies = NULL;
                close_connection(sockfd);
            }
        }


    }
    // Eliberarea memoriei
    if (cookies != NULL) {
        free(cookies);
    }
    if (token != NULL) {
        free(token);
    }
    if (message != NULL && response != NULL) {
        free(response);
        free(message);
    }
    close_connection(sockfd);

    return 0;
}
