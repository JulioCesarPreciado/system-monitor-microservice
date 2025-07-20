#include "../include/server.h"
#include "../include/system_info.h"
#include "../include/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

// Función para crear el socket del servidor
int create_server_socket(void) {
    int server_socket;
    struct sockaddr_in server_addr;
    int opt = 1;
    
    // Crear socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("❌ Error al crear socket");
        return -1;
    }
    
    // Permitir reutilizar la dirección
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("❌ Error en setsockopt");
        close(server_socket);
        return -1;
    }
    
    // Configurar dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Hacer bind del socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("❌ Error en bind");
        close(server_socket);
        return -1;
    }
    
    // Escuchar conexiones
    if (listen(server_socket, 5) < 0) {
        perror("❌ Error en listen");
        close(server_socket);
        return -1;
    }
    
    return server_socket;
}

// Función para enviar respuesta HTTP
void send_http_response(int client_socket, const char *content) {
    char http_response[MAX_RESPONSE + 512];
    
    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %lu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Server: SystemMonitor/1.0\r\n"
        "Cache-Control: no-cache\r\n"
        "\r\n"
        "%s",
        strlen(content), content
    );
    
    send(client_socket, http_response, strlen(http_response), 0);
}

// Función para enviar respuesta de error HTTP
void send_error_response(int client_socket, int error_code, const char *message) {
    char error_json[256];
    char http_response[512];
    
    snprintf(error_json, sizeof(error_json),
        "{\n"
        "  \"error\": %d,\n"
        "  \"message\": \"%s\",\n"
        "  \"platform\": \"%s\"\n"
        "}",
        error_code, message, get_platform_name()
    );
    
    snprintf(http_response, sizeof(http_response),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %lu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n"
        "%s",
        error_code, message, strlen(error_json), error_json
    );
    
    send(client_socket, http_response, strlen(http_response), 0);
}

// Función para manejar las conexiones de clientes
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    SystemInfo info;
    char response[MAX_RESPONSE];
    
    // Leer la petición del cliente (opcional, para logging)
    ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        // Podrías procesar diferentes endpoints aquí en el futuro
    }
    
    // Recopilar información del sistema
    collect_system_info(&info);
    
    // Formatear respuesta JSON
    format_json_response(&info, response, MAX_RESPONSE);
    
    // Enviar respuesta HTTP
    send_http_response(client_socket, response);
    
    // Cerrar conexión
    close(client_socket);
}

// Función principal para iniciar el servidor
void start_server(void) {
    int server_socket, client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    
    printf("🚀 Iniciando Microservicio de Monitoreo del Sistema\n");
    printf("═══════════════════════════════════════════════════\n");
    
    // Mostrar información de la plataforma
    extern void print_platform_info(void);
    print_platform_info();
    
    // Crear socket del servidor
    server_socket = create_server_socket();
    if (server_socket < 0) {
        fprintf(stderr, "❌ No se pudo crear el servidor\n");
        exit(1);
    }
    
    printf("📡 Servidor iniciado en puerto %d\n", PORT);
    printf("🌐 Accede a http://localhost:%d para obtener métricas\n", PORT);
    printf("🔄 El servidor detecta automáticamente el SO: %s\n", get_platform_name());
    printf("⏹️  Presiona Ctrl+C para detener el servidor\n\n");
    printf("📊 Esperando conexiones...\n");
    
    // Bucle principal del servidor
    while (1) {
        // Aceptar conexión de cliente
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            if (errno != EINTR) { // Ignorar interrupciones por señales
                perror("❌ Error al aceptar conexión");
            }
            continue;
        }
        
        printf("📡 Cliente conectado desde %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // Manejar cliente
        handle_client(client_socket);
        
        printf("✅ Respuesta enviada al cliente %s:%d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }
    
    close(server_socket);
}
