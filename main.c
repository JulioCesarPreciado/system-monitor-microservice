#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "include/server.h"
#include "include/platform.h"
#include "include/system_info.h"

// Variable global para manejar el cierre graceful
volatile sig_atomic_t server_running = 1;

// Manejador de señales para cierre graceful
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\n🛑 Señal de interrupción recibida (%d)\n", sig);
        printf("⏹️  Cerrando servidor de forma segura...\n");
        server_running = 0;
    }
}

// Función para mostrar información de ayuda
void print_help(const char *program_name) {
    printf("🖥️  Sistema de Monitoreo - Microservicio en C\n");
    printf("═════════════════════════════════════════════\n\n");
    printf("Uso: %s [opciones]\n\n", program_name);
    printf("Opciones:\n");
    printf("  -h, --help      Mostrar esta ayuda\n");
    printf("  -v, --version   Mostrar versión del programa\n");
    printf("  -p, --platform  Mostrar información de la plataforma\n");
    printf("  --processes     Mostrar análisis de procesos top y salir\n\n");
    printf("Ejemplos:\n");
    printf("  %s                 # Iniciar el servidor\n", program_name);
    printf("  %s --platform      # Ver información de la plataforma\n", program_name);
    printf("  %s --processes     # Análisis de procesos (ideal para servidores remotos)\n", program_name);
    printf("\nUna vez iniciado el servidor:\n");
    printf("  curl http://localhost:%d                     # Obtener métricas básicas\n", PORT);
    printf("  curl http://localhost:%d/processes/top       # Análisis de procesos\n", PORT);
    printf("  curl http://localhost:%d/help                # Documentación de API\n", PORT);
    printf("  curl -s http://localhost:%d | jq             # Con formato JSON\n", PORT);
    printf("\n🌐 Acceder desde navegador: http://localhost:%d\n", PORT);
    printf("\n🔍 Para análisis remoto de servidores:\n");
    printf("  ssh user@servidor '%s --processes'        # Análisis remoto directo\n", program_name);
}

// Función para mostrar versión
void print_version(void) {
    printf("Sistema de Monitoreo v1.1.0\n");
    printf("Plataforma: %s\n", get_platform_name());
    printf("Puerto: %d\n", PORT);
    printf("Nuevas características: Análisis de procesos top\n");
    printf("Compilado: %s %s\n", __DATE__, __TIME__);
}

// Función para mostrar información de plataforma
void print_platform_details(void) {
    printf("🖥️  Información de Plataforma\n");
    printf("════════════════════════════\n");
    printf("Sistema Operativo: %s\n", get_platform_name());
    printf("macOS Support: %s\n", is_macos() ? "✅ Sí" : "❌ No");
    printf("Linux Support: %s\n", is_linux() ? "✅ Sí" : "❌ No");
    
    // Mostrar rutas que se usarán
    printf("\nRutas de sistema utilizadas:\n");
    if (is_linux()) {
        printf("  • CPU Info: /proc/cpuinfo\n");
        printf("  • CPU Usage: /proc/stat\n");
        printf("  • Memory: /proc/meminfo\n");
        printf("  • Network: /proc/net/dev\n");
    } else if (is_macos()) {
        printf("  • CPU Info: sysctlbyname API\n");
        printf("  • CPU Usage: Mach API\n");
        printf("  • Memory: Mach VM API\n");
        printf("  • Network: ifconfig command\n");
    }
    
    printf("\nOptimizaciones activas:\n");
    printf("  • Detección automática de SO: ✅\n");
    printf("  • APIs nativas por plataforma: ✅\n");
    printf("  • Fallbacks multiplataforma: ✅\n");
}

int main(int argc, char *argv[]) {
    // Procesar argumentos de línea de comandos
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "--platform") == 0) {
            print_platform_details();
            return 0;
        } else if (strcmp(argv[1], "--processes") == 0) {
            // Nuevo flag para análisis de procesos
            printf("🔍 ANÁLISIS DE PROCESOS REMOTOS\n");
            printf("════════════════════════════════\n");
            TopProcesses top;
            get_top_processes(&top);
            display_top_processes(&top);
            return 0;
        } else {
            printf("❌ Opción desconocida: %s\n", argv[1]);
            printf("Usa '%s --help' para ver las opciones disponibles.\n", argv[0]);
            return 1;
        }
    }
    
    // Configurar manejadores de señales para cierre graceful
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Verificar plataforma soportada
    if (!is_macos() && !is_linux()) {
        printf("⚠️  Advertencia: Plataforma no completamente soportada (%s)\n", get_platform_name());
        printf("El servidor funcionará con funcionalidades limitadas.\n\n");
    }
    
    // Iniciar el servidor
    start_server();
    
    printf("\n👋 Servidor cerrado exitosamente\n");
    return 0;
}
