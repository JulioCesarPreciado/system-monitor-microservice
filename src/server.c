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
    char response[MAX_RESPONSE];
    
    // Leer la petición HTTP del cliente con un timeout adecuado
    ssize_t bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0); // Cambiar a bloqueante
    if (bytes_read <= 0) {
        // Si no se puede leer, enviar métricas básicas por defecto
        SystemInfo info;
        collect_system_info(&info);
        format_json_response(&info, response, MAX_RESPONSE);
        send_http_response(client_socket, response);
        close(client_socket);
        return;
    }
    
    buffer[bytes_read] = '\0';
    
    // Parsear la línea de petición HTTP para determinar el endpoint
    char method[16], path[256], version[16];
    if (sscanf(buffer, "%15s %255s %15s", method, path, version) != 3) {
        send_error_response(client_socket, 400, "Bad Request");
        close(client_socket);
        return;
    }
    
    printf("🔍 Request: %s %s\n", method, path);  // Debug temporal
    
    // Determinar qué endpoint se está solicitando
    if (strcmp(path, "/") == 0 || strcmp(path, "/metrics") == 0) {
        // Endpoint principal - métricas del sistema
        SystemInfo info;
        collect_system_info(&info);
        format_json_response(&info, response, MAX_RESPONSE);
        send_http_response(client_socket, response);
        
    } else if (strcmp(path, "/processes/top") == 0) {
        // Nuevo endpoint - análisis de procesos top
        TopProcesses top;
        get_top_processes(&top);
        format_processes_json_response(&top, response, MAX_RESPONSE);
        send_http_response(client_socket, response);
        
    } else if (strstr(path, "/help") != NULL || strstr(path, "/api") != NULL) {
        // Endpoint de ayuda/documentación de API
        snprintf(response, MAX_RESPONSE,
            "{\n"
            "  \"api_version\": \"1.1.0\",\n"
            "  \"platform\": \"%s\",\n"
            "  \"endpoints\": {\n"
            "    \"/\": {\n"
            "      \"description\": \"System metrics (CPU, RAM, Disk, Network)\",\n"
            "      \"method\": \"GET\"\n"
            "    },\n"
            "    \"/metrics\": {\n"
            "      \"description\": \"Alias for main endpoint\",\n"
            "      \"method\": \"GET\"\n"
            "    },\n"
            "    \"/processes/top\": {\n"
            "      \"description\": \"Top 10 processes by CPU, Memory and Disk usage\",\n"
            "      \"method\": \"GET\",\n"
            "      \"note\": \"Perfect for server analysis\"\n"
            "    },\n"
            "    \"/help\": {\n"
            "      \"description\": \"This help information\",\n"
            "      \"method\": \"GET\"\n"
            "    }\n"
            "  },\n"
            "  \"usage_examples\": {\n"
            "    \"basic_metrics\": \"curl http://localhost:%d/\",\n"
            "    \"process_analysis\": \"curl http://localhost:%d/processes/top\",\n"
            "    \"help_info\": \"curl http://localhost:%d/help\"\n"
            "  }\n"
            "}", 
            get_platform_name(), PORT, PORT, PORT);
        send_http_response(client_socket, response);
        
    } else {
        // Endpoint no encontrado
        snprintf(response, MAX_RESPONSE,
            "{\n"
            "  \"error\": 404,\n"
            "  \"message\": \"Endpoint not found: %s\",\n"
            "  \"available_endpoints\": [\"/\", \"/metrics\", \"/processes/top\", \"/help\"],\n"
            "  \"platform\": \"%s\"\n"
            "}", path, get_platform_name());
        
        char http_response[MAX_RESPONSE + 256];
        snprintf(http_response, sizeof(http_response),
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %lu\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "\r\n"
            "%s", strlen(response), response);
        
        send(client_socket, http_response, strlen(http_response), 0);
    }
    
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
