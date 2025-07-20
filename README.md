# Sistema de Monitoreo - Microservicio en C

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Linux-blue.svg)]()
[![C99](https://img.shields.io/badge/C-99-blue.svg)]()
[![Build Status](https://img.shields.io/badge/Build-Passing-green.svg)]()

🖥️ **Microservicio HTTP desarrollado en C** que proporciona métricas del sistema en tiempo real usando sockets TCP con detección automática de sistema operativo.

## ✨ Características Principales

- 🔍 **Detección automática de SO** (macOS/Linux)
- 📊 **Métricas en tiempo real**: CPU, RAM, Disco, Procesos, Red
- 🏗️ **Arquitectura modular** profesional
- 🚀 **APIs nativas** para máximo rendimiento
- 🌐 **Servidor HTTP** con sockets TCP
- 🧪 **Cliente de pruebas** incluido
- ⚙️ **Build system** avanzado

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
├── include/      # Headers (.h)
├── src/          # Código principal
├── utils/        # Utilidades
├── main.c        # Punto de entrada
└── Makefile      # Build system
```

## 🎯 Uso

### Como Microservicio
```bash
./system_monitor
curl http://localhost:8080
```

### Con Cliente Personalizado
```bash
./client_test
```

### Opciones del Programa
```bash
./system_monitor --help      # Ayuda
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

## 🤝 Contribuciones

¡Las contribuciones son bienvenidas! Por favor:

1. Fork el proyecto
2. Crea tu feature branch (`git checkout -b feature/nueva-funcionalidad`)
3. Commit tus cambios (`git commit -m 'Agregar nueva funcionalidad'`)
4. Push al branch (`git push origin feature/nueva-funcionalidad`)
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo la [Licencia MIT](LICENSE).

## 👨‍💻 Autor

**Julio César Preciado** - [@JulioCesarPreciado](https://github.com/JulioCesarPreciado)

---

⭐ **¡Dale una estrella si te gustó el proyecto!**
