#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

// Estructura para información de un proceso individual
typedef struct {
    int pid;
    char name[256];
    char cpu_usage[16];
    char memory_usage[16];
    char disk_usage[16];
    char user[64];
} ProcessInfo;

// Estructura para el top de procesos
typedef struct {
    ProcessInfo top_cpu[10];
    ProcessInfo top_memory[10]; 
    ProcessInfo top_disk[10];
    int cpu_count;
    int memory_count;
    int disk_count;
} TopProcesses;

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

// Nuevas funciones para análisis de procesos
void get_top_processes(TopProcesses *top);
void format_processes_json_response(TopProcesses *top, char *response, int max_size);
void display_top_processes(TopProcesses *top);

#endif // SYSTEM_INFO_H
