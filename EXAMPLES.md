# 📝 Ejemplos de Uso

## 🚀 Ejecución Básica

### 1. Compilar y Ejecutar
```bash
make
./system_monitor
```

### 2. Consultar Métricas
```bash
# En otra terminal
curl http://localhost:8080

# Con formato JSON legible
curl -s http://localhost:8080 | python -m json.tool
```

## 🧪 Usando el Cliente de Pruebas

```bash
# Compilar cliente
make

# Usar cliente personalizado
./client_test

# Salida esperada:
# 🔗 Conectado al servidor 127.0.0.1:8080
# 📡 Petición enviada, esperando respuesta...
# 📊 Información del sistema:
# {
#   "timestamp": "...",
#   "platform": "macOS",
#   ...
# }
```

## 🔧 Opciones del Programa

### Información del Sistema
```bash
./system_monitor --platform
```
```
🖥️  Información de Plataforma
════════════════════════════
Sistema Operativo: macOS
macOS Support: ✅ Sí
Linux Support: ❌ No

Rutas de sistema utilizadas:
  • CPU Info: sysctlbyname API
  • CPU Usage: Mach API
  • Memory: Mach VM API
  • Network: ifconfig command
```

### Versión
```bash
./system_monitor --version
```
```
Sistema de Monitoreo v1.0.0
Plataforma: macOS
Puerto: 8080
Compilado: Jul 20 2025 17:32:36
```

### Ayuda
```bash
./system_monitor --help
```

## 🌐 Integración con Aplicaciones Web

### JavaScript/Node.js
```javascript
// Función para obtener métricas del sistema
async function getSystemMetrics() {
    try {
        const response = await fetch('http://localhost:8080');
        const metrics = await response.json();
        console.log('CPU Usage:', metrics.hardware.cpu.usage);
        console.log('Memory Used:', metrics.hardware.memory.used);
        console.log('Processes:', metrics.system.processes);
        return metrics;
    } catch (error) {
        console.error('Error fetching metrics:', error);
    }
}

// Usar la función
getSystemMetrics();
```

### Python
```python
import requests
import json

def get_system_metrics():
    try:
        response = requests.get('http://localhost:8080')
        metrics = response.json()
        
        print(f"Platform: {metrics['platform']}")
        print(f"CPU: {metrics['hardware']['cpu']['model']}")
        print(f"CPU Usage: {metrics['hardware']['cpu']['usage']}")
        print(f"RAM Used: {metrics['hardware']['memory']['used']}")
        print(f"Processes: {metrics['system']['processes']}")
        
        return metrics
    except requests.exceptions.RequestException as e:
        print(f"Error: {e}")

# Usar la función
get_system_metrics()
```

### Bash/Shell
```bash
#!/bin/bash

# Monitor continuo cada 5 segundos
monitor_system() {
    while true; do
        echo "=== $(date) ==="
        curl -s http://localhost:8080 | \
            python -c "
import sys, json
data = json.load(sys.stdin)
print(f\"CPU: {data['hardware']['cpu']['usage']}\")
print(f\"RAM: {data['hardware']['memory']['used']}/{data['hardware']['memory']['total']}\")
print(f\"Processes: {data['system']['processes']}\")
"
        echo ""
        sleep 5
    done
}

monitor_system
```

## 📊 Monitoreo Avanzado

### Dashboard Simple con HTML
```html
<!DOCTYPE html>
<html>
<head>
    <title>System Monitor Dashboard</title>
    <script>
        async function updateMetrics() {
            try {
                const response = await fetch('http://localhost:8080');
                const data = await response.json();
                
                document.getElementById('cpu-usage').textContent = data.hardware.cpu.usage;
                document.getElementById('memory-used').textContent = data.hardware.memory.used;
                document.getElementById('processes').textContent = data.system.processes;
                document.getElementById('timestamp').textContent = data.timestamp;
            } catch (error) {
                console.error('Error:', error);
            }
        }
        
        // Actualizar cada 2 segundos
        setInterval(updateMetrics, 2000);
        updateMetrics(); // Primera carga
    </script>
</head>
<body>
    <h1>System Monitor</h1>
    <div>CPU Usage: <span id="cpu-usage">-</span></div>
    <div>Memory Used: <span id="memory-used">-</span></div>
    <div>Processes: <span id="processes">-</span></div>
    <div>Last Update: <span id="timestamp">-</span></div>
</body>
</html>
```

## ⚙️ Configuración Avanzada

### Cambiar Puerto
Edita `include/server.h`:
```c
#define PORT 9090  // Cambiar de 8080 a 9090
```

### Añadir Nueva Métrica
1. Editar `include/system_info.h` - Agregar campo a `SystemInfo`
2. Editar `src/system_info.c` - Implementar función de recolección
3. Editar función `format_json_response` - Incluir en JSON

### Ejemplo: Agregar Temperatura
```c
// En system_info.h
typedef struct {
    // ... campos existentes
    char cpu_temp[32];
} SystemInfo;

// En system_info.c
void get_cpu_temperature(char *cpu_temp) {
    // Implementación específica por SO
    #ifdef __APPLE__
        // macOS implementation
    #else
        // Linux implementation
    #endif
}
```

## 🧪 Pruebas y Debugging

### Compilación con Debug
```bash
make debug
```

### Análisis de Memory Leaks
```bash
# Si tienes Valgrind instalado
make memcheck
valgrind --leak-check=full ./system_monitor
```

### Análisis Estático
```bash
# Si tienes cppcheck instalado
make analyze
```

### Pruebas Automatizadas
```bash
# Ejecutar script de pruebas completo
./test_microservice.sh
```

## 📦 Distribución

### Crear Paquete
```bash
make package
```

### Instalar Globalmente
```bash
make install
# Ahora puedes usar: system_monitor
```

## 🔄 Desarrollo Continuo

### Workflow Típico
```bash
# 1. Hacer cambios en el código
vim src/system_info.c

# 2. Compilar y probar
make clean && make
./system_monitor --version

# 3. Probar funcionalidad
make test

# 4. Commit cambios
git add .
git commit -m "feat: agregar nueva métrica"
git push
```

¡Estos ejemplos te ayudarán a aprovechar al máximo tu microservicio de monitoreo! 🚀
