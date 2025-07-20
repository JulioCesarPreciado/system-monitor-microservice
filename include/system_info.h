#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

// Estructura principal para almacenar información del sistema
typedef struct {
    char cpu_model[256];
    char cpu_usage[32];
    char ram_total[32];
    char ram_used[32];
    char ram_free[32];
    char disk_total[32];
    char disk_used[32];
    char disk_free[32];
    int process_count;
    char public_ip[64];
    char network_status[128];
} SystemInfo;

// Funciones principales para recopilar información del sistema
void collect_system_info(SystemInfo *info);
void format_json_response(SystemInfo *info, char *response, int max_size);

// Funciones específicas de hardware
void get_cpu_model(char *cpu_model);
void get_cpu_usage(char *cpu_usage);
void get_memory_info(char *ram_total, char *ram_used, char *ram_free);
void get_disk_info(char *disk_total, char *disk_used, char *disk_free);

// Funciones específicas del sistema
int count_processes(void);
void get_public_ip(char *public_ip);
void get_network_status(char *network_status);

#endif // SYSTEM_INFO_H
