#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "cJSON.h"

char *create_monitor();

char *create_user();

char **extract_cookie(char *response, int *i);

char *extract_jwt_token(char *response);

char *get_acces_route();

char *create_book();

int main(int argc, char *argv[]) {
    char *message = NULL;
    char *response = NULL;
    int sockfd;

    sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);  // sock_streqam -> tcp
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }

    char **cookies = malloc(10 * sizeof(char));


    char *jwt_token = malloc(300 * sizeof(char));
    /*char** */ cookies = malloc(10 * sizeof(char));
    int cookies_counter = 0;
    char *command = malloc(20 * sizeof(char));
    char *access_route = NULL;
    char *user = NULL;
    char *book = NULL;

    while (1) {
        scanf("%s", command);
        sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);  // sock_streqam -> tcp
        if (sockfd == -1) {
            printf("socket creation failed...\n");
            exit(0);
        }
        if (strcmp(command, "exit") == 0)
            break;
        else if (strcmp(command, "register") == 0) {
            user = malloc(100 * sizeof(char));
            response = malloc(500 * sizeof(char));
            user = create_user();
            //printf("%s\n", user);
            message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                           "/api/v1/tema/auth/register", "application/json", user,
                                           0, NULL, 0, NULL);
            //printf("message: %s\n", message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
        } else if (strcmp(command, "login") == 0) {

            user = create_user();
            //printf("%s\n", user);
            message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                           "/api/v1/tema/auth/login", "application/json", user,
                                           0, cookies, 1, NULL);
            //printf("message: %s\n", message);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
            cookies = extract_cookie(response, &cookies_counter);
        } else if (strcmp(command, "enter_library") == 0) {
            message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                          "/api/v1/tema/library/access", NULL, cookies, 1, NULL);
            //printf("\n\nmessage: %s\n", message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
            jwt_token = extract_jwt_token(response);
        } else if (strcmp(command, "get_book") == 0) {
            access_route = get_acces_route();
            printf("access_route: %s\n", access_route);

            message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                          access_route, NULL, NULL, 1, jwt_token);
            //printf("\n\nmessage: %s\n", message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
        } else if (strcmp(command, "get_books") == 0) {
            message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                          "/api/v1/tema/library/books/", NULL, NULL, 0, jwt_token);
            //printf("\n\nmessage: %s\n", message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
        } else if (strcmp(command, "add_book") == 0) {
            book = create_book();

            message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                           "/api/v1/tema/library/books", "application/json", book,
                                           0, NULL, 0, jwt_token);
            //printf("message: %s\n", message);

            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
        } else if (strcmp(command, "delete_book") == 0) {

            access_route = get_acces_route();
            printf("access_route: %s\n", access_route);

            message = compute_delete_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                             access_route, NULL, NULL, 0, jwt_token);
            //printf("message: %s\n", message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
        } else if (strcmp(command, "logout") == 0) {
            message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                          "/api/v1/tema/auth/logout", NULL, cookies, 1, NULL);
            //printf("message: %s\n", message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
        }
    }

    free(user);
    free(book);
    if (response != NULL)
        free(response);
    if (message != NULL)
        free(message);
    free(command);
    free(access_route);
    free(jwt_token);

    close_connection(sockfd);
    return 0;
}

char *create_book() {
    char *string = NULL;
    cJSON *book = cJSON_CreateObject();
    cJSON *title = NULL;
    cJSON *author = NULL;
    cJSON *publisher = NULL;
    cJSON *genre = NULL;
    cJSON *page_count = NULL;

    char *token = NULL; //for strtok}

    char *title_str = malloc(40 * sizeof(char));
    scanf("%s", title_str);
    token = strtok(title_str, "=");
    token = strtok(NULL, "=");
    title = cJSON_CreateString(token);
    cJSON_AddItemToObject(book, "title", title);

    char *author_str = malloc(40 * sizeof(char));
    scanf("%s", author_str);
    token = strtok(author_str, "=");
    token = strtok(NULL, "=");
    author = cJSON_CreateString(token);
    cJSON_AddItemToObject(book, "author", author);

    char *genre_str = malloc(40 * sizeof(char));
    scanf("%s", genre_str);
    token = strtok(genre_str, "=");
    token = strtok(NULL, "=");
    genre = cJSON_CreateString(token);
    cJSON_AddItemToObject(book, "genre", genre);

    char *page_count_str = malloc(40 * sizeof(char));
    scanf("%s", page_count_str);
    token = strtok(page_count_str, "=");
    token = strtok(NULL, "=");
    page_count = cJSON_CreateNumber((double) atoi(token));
    cJSON_AddItemToObject(book, "page_count", page_count);

    char *publisher_str = malloc(40 * sizeof(char));
    scanf("%s", publisher_str);
    token = strtok(publisher_str, "=");
    token = strtok(NULL, "=");
    publisher = cJSON_CreateString(token);
    cJSON_AddItemToObject(book, "publisher", publisher);

    string = cJSON_Print(book);
    return string;
}

char *get_acces_route() {
    char *id = NULL;
    char *id_str = malloc(50 * sizeof(char));
    char *access_route = malloc(200 * sizeof(char));
    scanf("%s", id_str);
    id = strtok(id_str, "=");
    id = strtok(NULL, "=");
    strcat(access_route, "/api/v1/tema/library/books/");
    strcat(access_route, id);
    return access_route;
}

char *extract_jwt_token(char *response) {
    char *jwt_token = malloc(300 * sizeof(char));
    char *token;
    char *get_token;
    token = strtok(response, "\r\n");
    while (token != NULL) {
        if (strstr(token, "{\"token\":") != NULL) {
            get_token = strtok(token, "\"");
            get_token = strtok(NULL, "\"");
            get_token = strtok(NULL, "\"");
            get_token = strtok(NULL, "\"");
            strcpy(jwt_token, get_token);
        }

        token = strtok(NULL, "\r\n");
    }

    return jwt_token;
}

char **extract_cookie(char *response, int *i) {
    char *token;
    char *field;
    char *get_cookie;
    char **cookies = malloc(10 * sizeof(char));
    token = strtok(response, "\r\n");
    while (token != NULL) {
        if (strstr(token, "Set-Cookie: ") != NULL) {
            get_cookie = strtok(token, " ");
            get_cookie = strtok(NULL, " ");

            field = strtok(get_cookie, ";");
            while (field != NULL) {
                cookies[*i] = field;
                field = strtok(NULL, ";");
                (*i)++;
            }
            break;
        }

        token = strtok(NULL, "\r\n");
    }
    return cookies;
}


char *create_user() {

    char *string = NULL; // our JSON obj in string format
    cJSON *user = cJSON_CreateObject();
    cJSON *username = NULL;
    cJSON *password = NULL;

    char *token = NULL; //for strtok

    char *username_str = malloc(40 * sizeof(char));
    scanf("%s", username_str);
    token = strtok(username_str, "=");
    token = strtok(NULL, "=");
    username = cJSON_CreateString(token);
    cJSON_AddItemToObject(user, "username", username);


    char *password_str = malloc(40 * sizeof(char));
    scanf("%s", password_str);
    token = strtok(password_str, "=");
    token = strtok(NULL, "=");
    password = cJSON_CreateString(token);
    cJSON_AddItemToObject(user, "password", password);

    string = cJSON_Print(user);
    return string;
}