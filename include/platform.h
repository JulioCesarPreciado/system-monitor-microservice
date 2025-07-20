#ifndef PLATFORM_H
#define PLATFORM_H

// Detección de plataforma
#ifdef __APPLE__
    #define PLATFORM_MACOS 1
    #define PLATFORM_LINUX 0
#elif __linux__
    #define PLATFORM_MACOS 0
    #define PLATFORM_LINUX 1
#else
    #define PLATFORM_MACOS 0
    #define PLATFORM_LINUX 0
    #define PLATFORM_UNKNOWN 1
#endif

// Funciones para detección de plataforma
const char* get_platform_name(void);
int is_macos(void);
int is_linux(void);

// Constantes de rutas según la plataforma
#ifdef __APPLE__
    #define PROC_CPUINFO_PATH "/System/Library/CoreServices/SystemVersion.plist"
    #define PROC_STAT_PATH "/dev/null"
    #define PROC_MEMINFO_PATH "/dev/null"
    #define PROC_NET_DEV_PATH "/dev/null"
#else
    #define PROC_CPUINFO_PATH "/proc/cpuinfo"
    #define PROC_STAT_PATH "/proc/stat"
    #define PROC_MEMINFO_PATH "/proc/meminfo"
    #define PROC_NET_DEV_PATH "/proc/net/dev"
#endif

#endif // PLATFORM_H
