# 🎉 Primera Release Oficial - Sistema de Monitoreo v1.0.0

## 🚀 Características Principales

### 🖥️ **Microservicio HTTP Completo**
- **Servidor TCP/HTTP** en puerto 8080
- **JSON API** para métricas del sistema en tiempo real
- **CORS** habilitado para integraciones web
- **Cliente de pruebas** incluido

### 🔍 **Detección Automática de Sistema Operativo**
- ✅ **macOS** - APIs nativas (sysctlbyname, Mach APIs)
- ✅ **Linux** - Filesystem /proc y comandos del sistema  
- ✅ **Fallbacks** automáticos para máxima compatibilidad
- ✅ **Un solo ejecutable** para todas las plataformas

### 📊 **Métricas del Sistema en Tiempo Real**
- **CPU**: Modelo del procesador y porcentaje de uso actual
- **RAM**: Memoria total, usada y disponible (en GB)
- **Disco**: Espacio total, usado y libre del sistema raíz
- **Procesos**: Número exacto de procesos activos
- **Red**: IP local y número de interfaces de red activas
- **Timestamp**: Marca de tiempo de cada consulta

### 🏗️ **Arquitectura Modular Profesional**
- **Separación de responsabilidades** por módulos
- **Headers (.h) organizados** en directorio `include/`
- **Código fuente** estructurado en `src/`
- **Utilidades** auxiliares en `utils/`
- **Build system avanzado** con Makefile

## 📋 **Requisitos del Sistema**

- **Sistema Operativo**: macOS 10.12+ o Linux (cualquier distribución)
- **Compilador**: GCC 4.9+ o Clang 3.7+
- **Estándar C**: C99 compatible
- **Memoria**: Mínimo 1MB de RAM
- **Dependencias**: Solo bibliotecas estándar del sistema

## 🚀 **Instalación y Uso**

### Descarga Rápida
```bash
# Descargar y extraer
curl -L https://github.com/JulioCesarPreciado/system-monitor-microservice/archive/v1.0.0.tar.gz | tar xz
cd system-monitor-microservice-1.0.0

# Compilar
make

# Ejecutar
./system_monitor
```

### Desde Código Fuente
```bash
git clone https://github.com/JulioCesarPreciado/system-monitor-microservice.git
cd system-monitor-microservice
make
./system_monitor
```

### Verificar Instalación
```bash
./system_monitor --version
./system_monitor --platform
```

## 🧪 **Pruebas y Ejemplos**

### Obtener Métricas
```bash
# Método 1: curl
curl http://localhost:8080

# Método 2: Cliente incluido
./client_test

# Método 3: Navegador web
open http://localhost:8080
```

### Ejemplo de Respuesta JSON
```json
{
  "timestamp": "Sun Jul 20 17:47:55 2025",
  "platform": "macOS",
  "hardware": {
    "cpu": {
      "model": "Apple M3 Pro",
      "usage": "15.2%"
    },
    "memory": {
      "total": "18.00 GB",
      "used": "11.45 GB",
      "free": "6.55 GB"
    },
    "disk": {
      "total": "460.43 GB",
      "used": "245.58 GB",
      "free": "214.85 GB"
    }
  },
  "system": {
    "processes": 658,
    "network": {
      "ip": "192.168.1.114",
      "status": "25 network interfaces active"
    }
  }
}
```

## 🛠️ **Comandos del Build System**

```bash
make              # Compilar todo
make run          # Ejecutar servidor  
make test         # Pruebas básicas
make clean        # Limpiar archivos
make debug        # Compilar con debug
make help         # Ver todos los comandos
```

## 📚 **Documentación**

- **[README.md](README.md)** - Introducción y uso básico
- **[TECHNICAL_DOCS.md](TECHNICAL_DOCS.md)** - Documentación técnica detallada
- **[EXAMPLES.md](EXAMPLES.md)** - Ejemplos avanzados e integraciones
- **[LICENSE](LICENSE)** - Licencia MIT

## 🔧 **Características Técnicas**

### Rendimiento
- **Memoria**: Uso mínimo < 1MB
- **CPU**: Impacto negligible en el sistema
- **Latencia**: Respuestas < 50ms típicamente
- **Concurrencia**: Manejo de múltiples clientes simultáneos

### Seguridad
- **Sin privilegios root** requeridos
- **Puerto configurable** (por defecto 8080)
- **Sin almacenamiento** de datos persistentes
- **Código auditable** y abierto

### Compatibilidad
- **macOS**: 10.12 Sierra o superior
- **Linux**: Kernel 2.6+ (Ubuntu 16.04+, CentOS 7+, Debian 9+)
- **Arquitecturas**: x86_64, ARM64 (Apple Silicon)

## 🐛 **Problemas Conocidos y Soluciones**

### macOS
- **CPU Model**: Si aparece "Unknown CPU", es normal en algunos sistemas
- **Permisos**: No requiere permisos especiales

### Linux  
- **Proc filesystem**: Requiere acceso a `/proc/` (estándar)
- **Comandos**: Usa `ps`, `ifconfig` o `ip` según disponibilidad

### General
- **Puerto ocupado**: Cambiar puerto en `include/server.h` y recompilar
- **Firewall**: Permitir conexiones en puerto 8080

## 🚀 **Próximas Funcionalidades (Roadmap)**

- [ ] **HTTPS** support para conexiones seguras
- [ ] **Autenticación** con tokens API
- [ ] **Rate limiting** por cliente
- [ ] **Métricas históricas** con almacenamiento opcional
- [ ] **Dashboard web** integrado
- [ ] **Docker container** oficial
- [ ] **Windows** support

## 🤝 **Contribuciones**

¡Contribuciones bienvenidas! Ver [TECHNICAL_DOCS.md](TECHNICAL_DOCS.md) para guías de desarrollo.

## 📄 **Licencia**

MIT License - Ver [LICENSE](LICENSE) para detalles completos.

---

**¡Gracias por usar Sistema de Monitoreo! 🎉**

Si encuentras útil este proyecto, ⭐ **dale una estrella en GitHub** ⭐
