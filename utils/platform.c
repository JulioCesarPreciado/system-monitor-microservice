#include "../include/platform.h"
#include <stdio.h>

const char* get_platform_name(void) {
    #ifdef __APPLE__
        return "macOS";
    #elif __linux__
        return "Linux";
    #else
        return "Unknown";
    #endif
}

int is_macos(void) {
    #ifdef __APPLE__
        return 1;
    #else
        return 0;
    #endif
}

int is_linux(void) {
    #ifdef __linux__
        return 1;
    #else
        return 0;
    #endif
}

void print_platform_info(void) {
    printf("🖥️  Plataforma detectada: %s\n", get_platform_name());
    printf("📊 Sistema optimizado para esta plataforma\n");
}
