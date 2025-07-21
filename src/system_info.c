#include "../include/system_info.h"
#include "../include/platform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <time.h>
#include <dirent.h>

// Incluir headers específicos según la plataforma
#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#endif

// Función para obtener el modelo de CPU (multiplataforma)
void get_cpu_model(char *cpu_model) {
    if (is_macos()) {
        // macOS implementation
        #ifdef __APPLE__
        size_t size = 256;
        if (sysctlbyname("machdep.cpu.brand_string", cpu_model, &size, NULL, 0) == 0) {
            return;
        }
        #endif
    } else if (is_linux()) {
        // Linux implementation
        FILE *fp = fopen(PROC_CPUINFO_PATH, "r");
        if (fp == NULL) {
            strcpy(cpu_model, "Unknown CPU (Linux)");
            return;
        }
        
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "model name", 10) == 0) {
                char *start = strchr(line, ':');
                if (start) {
                    start += 2; // Saltar ": "
                    strcpy(cpu_model, start);
                    cpu_model[strcspn(cpu_model, "\n")] = 0;
                    fclose(fp);
                    return;
                }
            }
        }
        fclose(fp);
    }
    
    // Fallback
    strcpy(cpu_model, "Unknown CPU");
}

// Función para obtener el uso de CPU (multiplataforma)
void get_cpu_usage(char *cpu_usage) {
    if (is_macos()) {
        // macOS implementation
        #ifdef __APPLE__
        host_cpu_load_info_data_t cpuinfo;
        mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
        
        if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, 
                           (host_info_t)&cpuinfo, &count) == KERN_SUCCESS) {
            
            unsigned long long total_ticks = 0;
            unsigned long long idle_ticks = cpuinfo.cpu_ticks[CPU_STATE_IDLE];
            
            for (int i = 0; i < CPU_STATE_MAX; i++) {
                total_ticks += cpuinfo.cpu_ticks[i];
            }
            
            if (total_ticks > 0) {
                double cpu_percent = ((double)(total_ticks - idle_ticks) / total_ticks) * 100.0;
                snprintf(cpu_usage, 32, "%.1f%%", cpu_percent);
                return;
            }
        }
        #endif
    } else if (is_linux()) {
        // Linux implementation
        FILE *fp = fopen(PROC_STAT_PATH, "r");
        if (fp == NULL) {
            strcpy(cpu_usage, "Unknown");
            return;
        }
        
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
        if (fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) == 8) {
            
            unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
            unsigned long long work = total - idle;
            
            if (total > 0) {
                double cpu_percent = (work * 100.0) / total;
                snprintf(cpu_usage, 32, "%.1f%%", cpu_percent);
                fclose(fp);
                return;
            }
        }
        fclose(fp);
    }
    
    // Fallback usando comandos del sistema
    FILE *fp = popen("top -l 1 | grep 'CPU usage' | awk '{print $3}' 2>/dev/null || echo '0.0%'", "r");
    if (fp != NULL) {
        if (fgets(cpu_usage, 32, fp) != NULL) {
            cpu_usage[strcspn(cpu_usage, "\n")] = 0;
        } else {
            strcpy(cpu_usage, "0.0%");
        }
        pclose(fp);
    } else {
        strcpy(cpu_usage, "Unknown");
    }
}

// Función para obtener información de memoria RAM (multiplataforma)
void get_memory_info(char *ram_total, char *ram_used, char *ram_free) {
    if (is_macos()) {
        // macOS implementation
        #ifdef __APPLE__
        vm_statistics64_data_t vm_stat;
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
        
        if (host_statistics64(mach_host_self(), HOST_VM_INFO64, 
                             (host_info64_t)&vm_stat, &count) == KERN_SUCCESS) {
            
            // Obtener el tamaño de página
            vm_size_t page_size;
            if (host_page_size(mach_host_self(), &page_size) == KERN_SUCCESS) {
                // Obtener memoria total
                int64_t total_mem;
                size_t size = sizeof(total_mem);
                if (sysctlbyname("hw.memsize", &total_mem, &size, NULL, 0) == 0) {
                    
                    // Calcular memoria libre y usada
                    int64_t free_mem = (int64_t)(vm_stat.free_count + vm_stat.inactive_count) * page_size;
                    int64_t used_mem = total_mem - free_mem;
                    
                    snprintf(ram_total, 32, "%.2f GB", total_mem / 1024.0 / 1024.0 / 1024.0);
                    snprintf(ram_used, 32, "%.2f GB", used_mem / 1024.0 / 1024.0 / 1024.0);
                    snprintf(ram_free, 32, "%.2f GB", free_mem / 1024.0 / 1024.0 / 1024.0);
                    return;
                }
            }
        }
        #endif
    } else if (is_linux()) {
        // Linux implementation
        FILE *fp = fopen(PROC_MEMINFO_PATH, "r");
        if (fp == NULL) {
            strcpy(ram_total, "Unknown");
            strcpy(ram_used, "Unknown");
            strcpy(ram_free, "Unknown");
            return;
        }
        
        unsigned long mem_total = 0, mem_free = 0, mem_available = 0, buffers = 0, cached = 0;
        char line[256];
        
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "MemTotal: %lu kB", &mem_total) == 1) continue;
            if (sscanf(line, "MemFree: %lu kB", &mem_free) == 1) continue;
            if (sscanf(line, "MemAvailable: %lu kB", &mem_available) == 1) continue;
            if (sscanf(line, "Buffers: %lu kB", &buffers) == 1) continue;
            if (sscanf(line, "Cached: %lu kB", &cached) == 1) continue;
        }
        fclose(fp);
        
        unsigned long mem_used = mem_total - mem_free - buffers - cached;
        
        snprintf(ram_total, 32, "%.2f GB", mem_total / 1024.0 / 1024.0);
        snprintf(ram_used, 32, "%.2f GB", mem_used / 1024.0 / 1024.0);
        snprintf(ram_free, 32, "%.2f GB", mem_available / 1024.0 / 1024.0);
        return;
    }
    
    // Fallback
    strcpy(ram_total, "Unknown");
    strcpy(ram_used, "Unknown");
    strcpy(ram_free, "Unknown");
}

// Función para obtener información de disco
void get_disk_info(char *disk_total, char *disk_used, char *disk_free) {
    struct statvfs stat;
    
    if (statvfs("/", &stat) != 0) {
        strcpy(disk_total, "Unknown");
        strcpy(disk_used, "Unknown");
        strcpy(disk_free, "Unknown");
        return;
    }
    
    unsigned long long total = stat.f_blocks * stat.f_frsize;
    unsigned long long free = stat.f_bavail * stat.f_frsize;
    unsigned long long used = total - free;
    
    snprintf(disk_total, 32, "%.2f GB", total / 1024.0 / 1024.0 / 1024.0);
    snprintf(disk_used, 32, "%.2f GB", used / 1024.0 / 1024.0 / 1024.0);
    snprintf(disk_free, 32, "%.2f GB", free / 1024.0 / 1024.0 / 1024.0);
}

// Función para contar procesos activos (multiplataforma)
int count_processes(void) {
    if (is_macos()) {
        // macOS: usar ps para contar procesos
        FILE *fp = popen("ps -ax 2>/dev/null | wc -l", "r");
        if (fp == NULL) {
            return -1;
        }
        
        int count;
        if (fscanf(fp, "%d", &count) == 1) {
            pclose(fp);
            return count - 1; // Restar 1 por el header
        }
        pclose(fp);
    } else if (is_linux()) {
        // Linux: usar /proc
        DIR *proc_dir = opendir("/proc");
        if (proc_dir == NULL) {
            return -1;
        }
        
        struct dirent *entry;
        int count = 0;
        
        while ((entry = readdir(proc_dir)) != NULL) {
            if (strspn(entry->d_name, "0123456789") == strlen(entry->d_name)) {
                count++;
            }
        }
        
        closedir(proc_dir);
        return count;
    }
    
    return -1;
}

// Función para obtener IP local (multiplataforma)
void get_public_ip(char *public_ip) {
    FILE *fp;
    
    if (is_macos()) {
        fp = popen("ifconfig | grep 'inet ' | grep -v '127.0.0.1' | awk '{print $2}' | head -1", "r");
    } else {
        fp = popen("hostname -I | awk '{print $1}' 2>/dev/null || ip route get 8.8.8.8 | awk '{print $7; exit}'", "r");
    }
    
    if (fp == NULL) {
        strcpy(public_ip, "Unknown");
        return;
    }
    
    if (fgets(public_ip, 64, fp) != NULL) {
        public_ip[strcspn(public_ip, "\n")] = 0;
    } else {
        strcpy(public_ip, "Unknown");
    }
    
    pclose(fp);
}

// Función para obtener estado de red (multiplataforma)
void get_network_status(char *network_status) {
    FILE *fp;
    
    if (is_macos()) {
        fp = popen("ifconfig | grep '^[a-z]' | grep -v lo0 | wc -l", "r");
    } else {
        fp = popen("ip link show 2>/dev/null | grep '^[0-9]' | grep -v lo: | wc -l", "r");
    }
    
    if (fp == NULL) {
        strcpy(network_status, "Network info unavailable");
        return;
    }
    
    int interface_count;
    if (fscanf(fp, "%d", &interface_count) == 1) {
        snprintf(network_status, 128, "%d network interfaces active", interface_count);
    } else {
        strcpy(network_status, "Network info unavailable");
    }
    
    pclose(fp);
}

// Función para recopilar toda la información del sistema
void collect_system_info(SystemInfo *info) {
    get_cpu_model(info->cpu_model);
    get_cpu_usage(info->cpu_usage);
    get_memory_info(info->ram_total, info->ram_used, info->ram_free);
    get_disk_info(info->disk_total, info->disk_used, info->disk_free);
    info->process_count = count_processes();
    get_public_ip(info->public_ip);
    get_network_status(info->network_status);
}

// Función para formatear la respuesta JSON
void format_json_response(SystemInfo *info, char *response, int max_size) {
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strcspn(timestamp, "\n")] = 0; // Remover salto de línea
    
    snprintf(response, max_size,
        "{\n"
        "  \"timestamp\": \"%s\",\n"
        "  \"platform\": \"%s\",\n"
        "  \"hardware\": {\n"
        "    \"cpu\": {\n"
        "      \"model\": \"%s\",\n"
        "      \"usage\": \"%s\"\n"
        "    },\n"
        "    \"memory\": {\n"
        "      \"total\": \"%s\",\n"
        "      \"used\": \"%s\",\n"
        "      \"free\": \"%s\"\n"
        "    },\n"
        "    \"disk\": {\n"
        "      \"total\": \"%s\",\n"
        "      \"used\": \"%s\",\n"
        "      \"free\": \"%s\"\n"
        "    }\n"
        "  },\n"
        "  \"system\": {\n"
        "    \"processes\": %d,\n"
        "    \"network\": {\n"
        "      \"ip\": \"%s\",\n"
        "      \"status\": \"%s\"\n"
        "    }\n"
        "  }\n"
        "}",
        timestamp,
        get_platform_name(),
        info->cpu_model, info->cpu_usage,
        info->ram_total, info->ram_used, info->ram_free,
        info->disk_total, info->disk_used, info->disk_free,
        info->process_count,
        info->public_ip, info->network_status
    );
}

// Función para obtener los top 10 procesos por CPU (multiplataforma)
void get_top_processes_cpu(ProcessInfo *processes, int *count) {
    FILE *fp = NULL;
    *count = 0;
    
    if (is_macos()) {
        // macOS: usar ps con ordenamiento por CPU
        fp = popen("ps -axo pid,user,pcpu,comm | head -11 | tail -10", "r");
    } else if (is_linux()) {
        // Linux: usar ps con ordenamiento por CPU
        fp = popen("ps -axo pid,user,pcpu,comm --sort=-pcpu | head -11 | tail -10", "r");
    }
    
    if (fp == NULL) {
        return;
    }
    
    char line[512];
    int i = 0;
    
    while (fgets(line, sizeof(line), fp) && i < 10) {
        int pid;
        char user[64], cpu[16], name[256];
        
        if (sscanf(line, "%d %63s %15s %255s", &pid, user, cpu, name) == 4) {
            processes[i].pid = pid;
            strncpy(processes[i].name, name, sizeof(processes[i].name) - 1);
            strncpy(processes[i].cpu_usage, cpu, sizeof(processes[i].cpu_usage) - 1);
            strncpy(processes[i].user, user, sizeof(processes[i].user) - 1);
            strcpy(processes[i].memory_usage, "N/A");
            strcpy(processes[i].disk_usage, "N/A");
            
            processes[i].name[sizeof(processes[i].name) - 1] = '\0';
            processes[i].cpu_usage[sizeof(processes[i].cpu_usage) - 1] = '\0';
            processes[i].user[sizeof(processes[i].user) - 1] = '\0';
            
            i++;
        }
    }
    
    *count = i;
    pclose(fp);
}

// Función para obtener los top 10 procesos por memoria (multiplataforma)
void get_top_processes_memory(ProcessInfo *processes, int *count) {
    FILE *fp = NULL;
    *count = 0;
    
    if (is_macos()) {
        // macOS: usar ps con ordenamiento por memoria
        fp = popen("ps -axo pid,user,pmem,rss,comm | head -11 | tail -10", "r");
    } else if (is_linux()) {
        // Linux: usar ps con ordenamiento por memoria
        fp = popen("ps -axo pid,user,pmem,rss,comm --sort=-pmem | head -11 | tail -10", "r");
    }
    
    if (fp == NULL) {
        return;
    }
    
    char line[512];
    int i = 0;
    
    while (fgets(line, sizeof(line), fp) && i < 10) {
        int pid, rss;
        char user[64], mem[16], name[256];
        
        if (sscanf(line, "%d %63s %15s %d %255s", &pid, user, mem, &rss, name) == 5) {
            processes[i].pid = pid;
            strncpy(processes[i].name, name, sizeof(processes[i].name) - 1);
            snprintf(processes[i].memory_usage, sizeof(processes[i].memory_usage), "%.1fMB", rss / 1024.0);
            strncpy(processes[i].user, user, sizeof(processes[i].user) - 1);
            strcpy(processes[i].cpu_usage, "N/A");
            strcpy(processes[i].disk_usage, "N/A");
            
            processes[i].name[sizeof(processes[i].name) - 1] = '\0';
            processes[i].user[sizeof(processes[i].user) - 1] = '\0';
            
            i++;
        }
    }
    
    *count = i;
    pclose(fp);
}

// Función para obtener los top 10 procesos por I/O de disco (multiplataforma)
void get_top_processes_disk(ProcessInfo *processes, int *count) {
    FILE *fp = NULL;
    *count = 0;
    
    if (is_macos()) {
        // macOS: usar iotop o ps (limitado)
        fp = popen("ps -axo pid,user,comm | head -11 | tail -10", "r");
    } else if (is_linux()) {
        // Linux: intentar usar iotop, fallback a ps
        fp = popen("which iotop > /dev/null 2>&1 && iotop -b -n 1 -P -o | head -10 || ps -axo pid,user,comm | head -11 | tail -10", "r");
    }
    
    if (fp == NULL) {
        return;
    }
    
    char line[512];
    int i = 0;
    
    while (fgets(line, sizeof(line), fp) && i < 10) {
        int pid;
        char user[64], name[256];
        
        // Formato básico cuando no hay iotop disponible
        if (sscanf(line, "%d %63s %255s", &pid, user, name) == 3) {
            processes[i].pid = pid;
            strncpy(processes[i].name, name, sizeof(processes[i].name) - 1);
            strncpy(processes[i].user, user, sizeof(processes[i].user) - 1);
            strcpy(processes[i].cpu_usage, "N/A");
            strcpy(processes[i].memory_usage, "N/A");
            strcpy(processes[i].disk_usage, "Limited");
            
            processes[i].name[sizeof(processes[i].name) - 1] = '\0';
            processes[i].user[sizeof(processes[i].user) - 1] = '\0';
            
            i++;
        }
    }
    
    *count = i;
    pclose(fp);
}

// Función principal para obtener todos los top processes
void get_top_processes(TopProcesses *top) {
    get_top_processes_cpu(top->top_cpu, &top->cpu_count);
    get_top_processes_memory(top->top_memory, &top->memory_count);
    get_top_processes_disk(top->top_disk, &top->disk_count);
}

// Función para formatear la respuesta JSON de procesos
void format_processes_json_response(TopProcesses *top, char *response, int max_size) {
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strcspn(timestamp, "\n")] = 0;
    
    int offset = 0;
    
    // Cabecera JSON
    offset += snprintf(response + offset, max_size - offset,
        "{\n"
        "  \"timestamp\": \"%s\",\n"
        "  \"platform\": \"%s\",\n"
        "  \"analysis\": {\n"
        "    \"top_cpu_processes\": [\n", 
        timestamp, get_platform_name());
    
    // Top procesos por CPU
    for (int i = 0; i < top->cpu_count && offset < max_size - 200; i++) {
        offset += snprintf(response + offset, max_size - offset,
            "      {\n"
            "        \"pid\": %d,\n"
            "        \"name\": \"%s\",\n"
            "        \"user\": \"%s\",\n"
            "        \"cpu_usage\": \"%s%%\"\n"
            "      }%s\n",
            top->top_cpu[i].pid,
            top->top_cpu[i].name,
            top->top_cpu[i].user,
            top->top_cpu[i].cpu_usage,
            (i < top->cpu_count - 1) ? "," : "");
    }
    
    offset += snprintf(response + offset, max_size - offset,
        "    ],\n"
        "    \"top_memory_processes\": [\n");
    
    // Top procesos por memoria
    for (int i = 0; i < top->memory_count && offset < max_size - 200; i++) {
        offset += snprintf(response + offset, max_size - offset,
            "      {\n"
            "        \"pid\": %d,\n"
            "        \"name\": \"%s\",\n"
            "        \"user\": \"%s\",\n"
            "        \"memory_usage\": \"%s\"\n"
            "      }%s\n",
            top->top_memory[i].pid,
            top->top_memory[i].name,
            top->top_memory[i].user,
            top->top_memory[i].memory_usage,
            (i < top->memory_count - 1) ? "," : "");
    }
    
    offset += snprintf(response + offset, max_size - offset,
        "    ],\n"
        "    \"top_disk_processes\": [\n");
    
    // Top procesos por disco
    for (int i = 0; i < top->disk_count && offset < max_size - 200; i++) {
        offset += snprintf(response + offset, max_size - offset,
            "      {\n"
            "        \"pid\": %d,\n"
            "        \"name\": \"%s\",\n"
            "        \"user\": \"%s\",\n"
            "        \"disk_status\": \"%s\"\n"
            "      }%s\n",
            top->top_disk[i].pid,
            top->top_disk[i].name,
            top->top_disk[i].user,
            top->top_disk[i].disk_usage,
            (i < top->disk_count - 1) ? "," : "");
    }
    
    // Cierre JSON
    snprintf(response + offset, max_size - offset,
        "    ]\n"
        "  },\n"
        "  \"summary\": {\n"
        "    \"total_analyzed_processes\": %d,\n"
        "    \"platform_capabilities\": \"%s\"\n"
        "  }\n"
        "}",
        top->cpu_count + top->memory_count + top->disk_count,
        is_linux() ? "Full process analysis available" : "Basic process analysis available");
}

// Función para mostrar los top processes en consola
void display_top_processes(TopProcesses *top) {
    printf("\n🔍 TOP PROCESSES ANALYSIS - %s\n", get_platform_name());
    printf("=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "=" "\n");
    
    // Top CPU
    printf("\n🚀 TOP 10 PROCESSES BY CPU USAGE:\n");
    printf("%-8s %-15s %-8s %s\n", "PID", "USER", "CPU%", "PROCESS NAME");
    printf("-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "\n");
    
    for (int i = 0; i < top->cpu_count; i++) {
        printf("%-8d %-15s %-8s %s\n",
            top->top_cpu[i].pid,
            top->top_cpu[i].user,
            top->top_cpu[i].cpu_usage,
            top->top_cpu[i].name);
    }
    
    // Top Memory
    printf("\n🧠 TOP 10 PROCESSES BY MEMORY USAGE:\n");
    printf("%-8s %-15s %-12s %s\n", "PID", "USER", "MEMORY", "PROCESS NAME");
    printf("-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "\n");
    
    for (int i = 0; i < top->memory_count; i++) {
        printf("%-8d %-15s %-12s %s\n",
            top->top_memory[i].pid,
            top->top_memory[i].user,
            top->top_memory[i].memory_usage,
            top->top_memory[i].name);
    }
    
    // Top Disk
    printf("\n💾 TOP 10 PROCESSES BY DISK ACTIVITY:\n");
    printf("%-8s %-15s %-12s %s\n", "PID", "USER", "DISK I/O", "PROCESS NAME");
    printf("-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "-" "\n");
    
    for (int i = 0; i < top->disk_count; i++) {
        printf("%-8d %-15s %-12s %s\n",
            top->top_disk[i].pid,
            top->top_disk[i].user,
            top->top_disk[i].disk_usage,
            top->top_disk[i].name);
    }
    
    printf("\n📊 Analysis completed for %d total processes\n\n",
        top->cpu_count + top->memory_count + top->disk_count);
}
