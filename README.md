# Sistema de Monitoreo - Microservicio en C

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Linux-blue.svg)]()
[![C99](https://img.shields.io/badge/C-99-blue.svg)]()
[![Build Status](https://img.shields.io/badge/Build-Passing-green.svg)]()
[![Version](https://img.shields.io/badge/Version-1.1.0-blue.svg)]()

🖥️ **Microservicio HTTP desarrollado en C** que proporciona métricas del sistema en tiempo real usando sockets TCP con detección automática de sistema operativo.

## ✨ Características Principales

- 🔍 **Detección automática de SO** (macOS/Linux)
- 📊 **Métricas en tiempo real**: CPU, RAM, Disco, Procesos, Red
- 🚀 **NUEVO: Análisis de procesos top** - Ideal para servidores remotos
- 🏗️ **Arquitectura modular** profesional
- ⚡ **APIs nativas** para máximo rendimiento
- 🌐 **Servidor HTTP** con múltiples endpoints
- 🔧 **CLI flags** para análisis directo
- 🧪 **Cliente de pruebas** incluido
- ⚙️ **Build system** avanzado

## 🆕 Nuevas Funcionalidades v1.1.0

### 🔥 Análisis de Procesos Top
- **Top 10 procesos por CPU**: Identifica procesos que más consumen CPU
- **Top 10 procesos por memoria**: Encuentra memory leaks y procesos pesados  
- **Top 10 procesos por disco**: Analiza actividad I/O en tiempo real
- **Análisis remoto**: Perfecto para monitoreo de servidores via SSH
- **Endpoint HTTP**: `/processes/top` para integración con dashboards

### 🚀 Casos de Uso
- **Análisis de servidores remotos**: `ssh server './system_monitor --processes'`
- **Monitoreo automático**: APIs HTTP para dashboards
- **Troubleshooting**: Identificar procesos problemáticos rápidamente
- **Optimización**: Detectar cuellos de botella en producción

## 🚀 Instalación Rápida

```bash
# Clonar el repositorio
git clone https://github.com/JulioCesarPreciado/system-monitor-microservice.git
cd system-monitor-microservice

# Compilar
make

# Ejecutar
./system_monitor
```

## 📊 Ejemplo de Respuesta

```json
{
  "timestamp": "Sun Jul 20 16:54:33 2025",
  "platform": "macOS",
  "hardware": {
    "cpu": {
      "model": "Apple M3 Pro",
      "usage": "13.0%"
    },
    "memory": {
      "total": "18.00 GB",
      "used": "11.54 GB",
      "free": "6.46 GB"
    },
    "disk": {
      "total": "460.43 GB",
      "used": "245.62 GB",
      "free": "214.81 GB"
    }
  },
  "system": {
    "processes": 665,
    "network": {
      "ip": "192.168.1.114",
      "status": "25 network interfaces active"
    }
  }
}
```

## 🆕 Ejemplo de Análisis de Procesos (v1.1.0)

### Análisis Directo via CLI
```bash
# Análisis local
./system_monitor --processes

# Análisis remoto via SSH
ssh usuario@servidor './system_monitor --processes'

# Usando el script automatizado
./remote_analysis.sh servidor.example.com admin 22
```

### Análisis via HTTP API
```bash
# Iniciar servidor
./system_monitor &

# Obtener análisis de procesos top
curl http://localhost:8080/processes/top | jq

# Respuesta ejemplo (resumida):
{
  "timestamp": "Sun Jul 20 18:32:52 2025",
  "platform": "macOS", 
  "analysis": {
    "top_cpu_processes": [
      {
        "pid": 312,
        "name": "/System/Library/CoreServices/powerd.bundle/powerd",
        "user": "root",
        "cpu_usage": "0.4%"
      }
      // ... 9 procesos más
    ],
    "top_memory_processes": [
      {
        "pid": 299,
        "name": "/usr/libexec/logd", 
        "user": "root",
        "memory_usage": "42.2MB"
      }
      // ... 9 procesos más  
    ],
    "top_disk_processes": [
      // Procesos con mayor actividad I/O
    ]
  },
  "summary": {
    "total_analyzed_processes": 30,
    "platform_capabilities": "Basic process analysis available"
  }
}

# Obtener ayuda de la API
curl http://localhost:8080/help | jq
```

## 🛠️ Comandos Disponibles

```bash
make              # Compilar todo
make run          # Ejecutar servidor  
make test         # Pruebas básicas
make clean        # Limpiar archivos
make help         # Ver todas las opciones
```

## 📁 Estructura del Proyecto

```
system-monitor-microservice/
├── include/              # Headers (.h)
│   ├── system_info.h    # Estructuras y funciones de sistema
│   ├── server.h         # Servidor HTTP
│   └── platform.h       # Detección de plataforma
├── src/                 # Código principal
│   ├── system_info.c    # Recolección de métricas y procesos
│   └── server.c         # Servidor HTTP con múltiples endpoints
├── utils/               # Utilidades
│   └── platform.c       # Detección automática de SO
├── main.c               # Punto de entrada con nuevos flags
├── remote_analysis.sh   # Script para análisis remoto
└── Makefile             # Build system avanzado
```

## 🎯 Uso

### 🚀 Como Microservicio HTTP
```bash
# Iniciar servidor
./system_monitor

# Endpoints disponibles:
curl http://localhost:8080/                 # Métricas básicas
curl http://localhost:8080/processes/top    # Análisis de procesos ⭐ NUEVO
curl http://localhost:8080/help             # Documentación API
```

### 🔧 Análisis Directo (CLI)
```bash
./system_monitor --help        # Ayuda completa
./system_monitor --version     # Información de versión
./system_monitor --platform    # Info de plataforma
./system_monitor --processes   # Análisis de procesos top ⭐ NUEVO
```

### 🌐 Análisis Remoto de Servidores
```bash
# Método 1: SSH directo
ssh admin@prod-server './system_monitor --processes'

# Método 2: Script automatizado  
./remote_analysis.sh prod-server admin 2222

# Método 3: API HTTP remota
ssh admin@prod-server './system_monitor &'
curl http://prod-server:8080/processes/top
```

### 🧪 Con Cliente Personalizado
```bash
./client_test
./system_monitor --version   # Versión
./system_monitor --platform  # Info del SO
```

## 📋 Requisitos

- **SO**: macOS o Linux
- **Compilador**: GCC o Clang
- **Estándar**: C99

## 🏗️ Arquitectura

- **Modular**: Separación clara de responsabilidades
- **Multiplataforma**: Detección automática de APIs
- **Eficiente**: Uso de APIs nativas del sistema
- **Extensible**: Fácil agregar nuevas métricas

## 📋 Mantenimiento de Versiones

### Release Notes
Este proyecto mantiene un archivo único [RELEASE_NOTES.md](RELEASE_NOTES.md) con el historial completo de versiones. Para agregar una nueva versión:

```bash
# Usar el script automatizado
./update_release_notes.sh v1.2.0 "Descripción de la nueva versión"

# Editar el archivo para agregar detalles específicos
# Hacer commit y crear release
git add RELEASE_NOTES.md
git commit -m "docs: Update release notes for v1.2.0"
gh release create v1.2.0 --notes-file RELEASE_NOTES.md
```

### Versionado
Seguimos [Semantic Versioning](https://semver.org/):
- **MAJOR** (v2.0.0): Cambios incompatibles en la API
- **MINOR** (v1.1.0): Nuevas funcionalidades compatibles
- **PATCH** (v1.0.1): Correcciones de bugs

## 🤝 Contribuciones

¡Las contribuciones son bienvenidas! Por favor:

1. Fork el proyecto
2. Crea tu feature branch (`git checkout -b feature/nueva-funcionalidad`)
3. Commit tus cambios (`git commit -m 'Agregar nueva funcionalidad'`)
4. Push al branch (`git push origin feature/nueva-funcionalidad`)
5. Abre un Pull Request

## � Documentación Adicional

- 📖 **[TECHNICAL_DOCS.md](TECHNICAL_DOCS.md)** - Documentación técnica detallada y guía de desarrollo
- 🧪 **[EXAMPLES.md](EXAMPLES.md)** - Ejemplos avanzados de uso e integración
- ⚙️ **[Makefile](Makefile)** - Ver `make help` para todos los comandos disponibles

## �📄 Licencia

Este proyecto está bajo la [Licencia MIT](LICENSE).

## 👨‍💻 Autor

**Julio César Preciado** - [@JulioCesarPreciado](https://github.com/JulioCesarPreciado)

---

⭐ **¡Dale una estrella si te gustó el proyecto!**
