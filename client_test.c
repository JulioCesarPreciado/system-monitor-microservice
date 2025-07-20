#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char *server_ip = "127.0.0.1";  // localhost por defecto
    
    // Permitir especificar IP del servidor como argumento
    if (argc > 1) {
        server_ip = argv[1];
    }
    
    // Crear socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error al crear socket");
        exit(1);
    }
    
    // Configurar dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Dirección IP inválida");
        exit(1);
    }
    
    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar");
        printf("Asegúrate de que el servidor esté ejecutándose en %s:%d\n", server_ip, SERVER_PORT);
        exit(1);
    }
    
    printf("🔗 Conectado al servidor %s:%d\n", server_ip, SERVER_PORT);
    
    // Enviar petición HTTP GET
    const char* request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send(client_socket, request, strlen(request), 0);
    
    printf("📡 Petición enviada, esperando respuesta...\n\n");
    
    // Recibir y mostrar respuesta
    ssize_t bytes_received;
    int header_ended = 0;
    
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        
        if (!header_ended) {
            // Buscar el final de los headers HTTP
            char *body_start = strstr(buffer, "\r\n\r\n");
            if (body_start) {
                body_start += 4; // Saltar "\r\n\r\n"
                printf("📊 Información del sistema:\n");
                printf("%s", body_start);
                header_ended = 1;
            }
        } else {
            printf("%s", buffer);
        }
    }
    
    if (bytes_received < 0) {
        perror("Error al recibir datos");
    }
    
    printf("\n\n✅ Conexión cerrada exitosamente\n");
    
    close(client_socket);
    return 0;
}
