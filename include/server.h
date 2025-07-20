#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

// Configuración del servidor
#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_RESPONSE 8192

// Funciones del servidor HTTP
int create_server_socket(void);
void handle_client(int client_socket);
void start_server(void);

// Utilidades HTTP
void send_http_response(int client_socket, const char *content);
void send_error_response(int client_socket, int error_code, const char *message);

#endif // SERVER_H
