# � Documentación Técnica Detallada

> **Nota**: Este archivo contiene documentación técnica profunda. Para una introducción rápida, consulta [README.md](README.md).

# �🖥️ Sistema de Monitoreo - Microservicio en C

Un microservicio desarrollado en C que proporciona información detallada sobre el hardware y estado del sistema a través de un servidor HTTP usando sockets TCP.

## 📋 Características

### Hardware Information
- **CPU**: Modelo del procesador y uso actual
- **RAM**: Memoria total, usada y disponible
- **Disco**: Espacio total, usado y libre

### System Metrics
- **Procesos**: Número de procesos activos en el sistema
- **Red**: IP del servidor y estado de interfaces de red
- **Timestamp**: Marca de tiempo de cuando se obtuvo la información

## 🚀 Instalación y Uso

### Prerrequisitos
- Sistema operativo Linux o macOS
- Compilador GCC
- Acceso a `/proc` filesystem (Linux)

### Compilación
```bash
# Clonar o descargar el proyecto
cd analitycs_services

# Compilar el programa
make

# O compilar manualmente
gcc -Wall -Wextra -std=c99 -pedantic -o system_monitor system_monitor.c
```

### Ejecución
```bash
# Ejecutar el microservicio
make run

# O ejecutar directamente
./system_monitor
```

El servidor se iniciará en el puerto **8080** y estará listo para recibir conexiones.

## 🌐 API Endpoints

### GET /
Retorna información completa del sistema en formato JSON.

**Respuesta ejemplo:**
```json
{
  "timestamp": "Sun Jul 20 10:30:45 2025",
  "hardware": {
    "cpu": {
      "model": "Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz",
      "usage": "15.3%"
    },
    "memory": {
      "total": "16.00 GB",
      "used": "8.45 GB",
      "free": "6.12 GB"
    },
    "disk": {
      "total": "500.00 GB",
      "used": "245.67 GB",
      "free": "230.89 GB"
    }
  },
  "system": {
    "processes": 156,
    "network": {
      "ip": "192.168.1.100",
      "status": "3 network interfaces active"
    }
  }
}
```

## 🧪 Pruebas

### Usando curl
```bash
# Obtener métricas del sistema
curl http://localhost:8080

# Con formato bonito
curl -s http://localhost:8080 | python -m json.tool
```

### Usando navegador web
Abre tu navegador y visita: `http://localhost:8080`

### Usando herramientas de desarrollo
```bash
# Monitoreo continuo cada 5 segundos
watch -n 5 "curl -s http://localhost:8080 | python -m json.tool"
```

## 🏗️ Arquitectura

### Estructura del código
```
system_monitor.c
├── Estructuras de datos
│   └── SystemInfo - Almacena toda la información del sistema
├── Funciones de recolección de datos
│   ├── get_cpu_model() - Obtiene modelo del CPU
│   ├── get_cpu_usage() - Calcula uso de CPU
│   ├── get_memory_info() - Información de RAM
│   ├── get_disk_info() - Información de disco
│   ├── count_processes() - Cuenta procesos activos
│   ├── get_public_ip() - Obtiene IP del sistema
│   └── get_network_status() - Estado de la red
├── Funciones de servidor
│   ├── format_json_response() - Formatea respuesta JSON
│   ├── handle_client() - Maneja conexiones de clientes
│   └── main() - Bucle principal del servidor
```

### Flujo de datos
1. Cliente realiza conexión TCP al puerto 8080
2. Servidor acepta la conexión
3. Se recopila información del sistema leyendo `/proc/*`
4. Los datos se formatean en JSON
5. Se envía respuesta HTTP con Content-Type: application/json
6. Se cierra la conexión

## 🔧 Personalización

### Cambiar puerto
Modifica la constante `PORT` en el archivo `system_monitor.c`:
```c
#define PORT 8080  // Cambia a tu puerto preferido
```

### Añadir nuevas métricas
1. Agrega campos a la estructura `SystemInfo`
2. Crea función para recopilar los datos
3. Llama la función en `collect_system_info()`
4. Actualiza `format_json_response()` para incluir los nuevos datos

### Ejemplo: Añadir temperatura de CPU
```c
// En la estructura SystemInfo
char cpu_temp[32];

// Nueva función
void get_cpu_temperature(char *cpu_temp) {
    FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (fp == NULL) {
        strcpy(cpu_temp, "Unknown");
        return;
    }
    
    int temp;
    fscanf(fp, "%d", &temp);
    fclose(fp);
    
    snprintf(cpu_temp, 32, "%.1f°C", temp / 1000.0);
}
```

## 🐛 Troubleshooting

### Error: "Permission denied"
```bash
# Asegúrate de que el puerto no esté en uso
sudo lsof -i :8080

# O usa un puerto diferente (> 1024)
```

### Error: "Address already in use"
```bash
# Mata procesos usando el puerto
sudo pkill -f system_monitor

# O espera unos segundos para que se libere el puerto
```

### Datos incorrectos en Linux vs macOS
El programa está optimizado para Linux. En macOS, algunas métricas pueden no estar disponibles porque usa el filesystem `/proc` que es específico de Linux.

## 🚀 Características Avanzadas

### Para producción considera:
- **Autenticación**: Añadir tokens de API
- **Rate limiting**: Limitar número de requests por cliente
- **Logging**: Registrar todas las conexiones y errores
- **Configuración**: Archivo de configuración externo
- **Daemon mode**: Ejecutar como servicio del sistema
- **HTTPS**: Soporte para conexiones seguras

### Monitoreo en tiempo real
```bash
# Crear script para monitoreo continuo
#!/bin/bash
while true; do
    echo "=== $(date) ==="
    curl -s http://localhost:8080 | python -m json.tool
    echo ""
    sleep 10
done
```

## 📝 Comandos útiles

```bash
# Compilar con información de debug
gcc -g -Wall -Wextra -std=c99 -pedantic -o system_monitor system_monitor.c

# Ejecutar con Valgrind para detectar memory leaks
valgrind --leak-check=full ./system_monitor

# Compilar optimizado para producción
gcc -O2 -Wall -Wextra -std=c99 -pedantic -o system_monitor system_monitor.c

# Limpiar archivos temporales
make clean
```

## 🤝 Contribución

1. Fork el proyecto
2. Crea una branch para tu feature (`git checkout -b feature/nueva-metrica`)
3. Commit tus cambios (`git commit -am 'Añadir nueva métrica'`)
4. Push a la branch (`git push origin feature/nueva-metrica`)
5. Crea un Pull Request

## 📄 License

Este proyecto está bajo la licencia MIT. Puedes usarlo libremente para aprender y desarrollar.

---

**¡Feliz coding! 🚀**
