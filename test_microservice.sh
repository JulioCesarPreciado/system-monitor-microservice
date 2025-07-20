#!/bin/bash

# Script para probar el microservicio de monitoreo del sistema
# Uso: ./test_microservice.sh

echo "🧪 Iniciando pruebas del microservicio de monitoreo del sistema"
echo "=============================================================="

# Colores para output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Función para verificar si un puerto está en uso
check_port() {
    if lsof -Pi :8080 -sTCP:LISTEN -t >/dev/null 2>&1; then
        return 0  # Puerto en uso
    else
        return 1  # Puerto libre
    fi
}

# Compilar el proyecto
echo -e "${YELLOW}📦 Compilando el proyecto...${NC}"
if make clean && make; then
    echo -e "${GREEN}✅ Compilación exitosa${NC}"
else
    echo -e "${RED}❌ Error en la compilación${NC}"
    exit 1
fi

# Verificar si el servidor ya está ejecutándose
if check_port; then
    echo -e "${YELLOW}⚠️  El servidor ya está ejecutándose en el puerto 8080${NC}"
    echo "Continuando con las pruebas..."
else
    # Iniciar el servidor en background
    echo -e "${YELLOW}🚀 Iniciando el servidor...${NC}"
    ./system_monitor &
    SERVER_PID=$!
    
    # Esperar un momento para que el servidor se inicie
    sleep 2
    
    # Verificar que el servidor se inició correctamente
    if check_port; then
        echo -e "${GREEN}✅ Servidor iniciado correctamente (PID: $SERVER_PID)${NC}"
    else
        echo -e "${RED}❌ Error al iniciar el servidor${NC}"
        exit 1
    fi
fi

echo ""
echo -e "${YELLOW}🧪 Ejecutando pruebas...${NC}"
echo ""

# Prueba 1: Conectividad básica
echo "1️⃣  Prueba de conectividad básica:"
if curl -s http://localhost:8080 > /dev/null; then
    echo -e "${GREEN}   ✅ Servidor responde correctamente${NC}"
else
    echo -e "${RED}   ❌ Servidor no responde${NC}"
fi

# Prueba 2: Formato de respuesta JSON
echo ""
echo "2️⃣  Prueba de formato JSON:"
RESPONSE=$(curl -s http://localhost:8080)
if echo "$RESPONSE" | python -m json.tool > /dev/null 2>&1; then
    echo -e "${GREEN}   ✅ Respuesta es JSON válido${NC}"
else
    echo -e "${RED}   ❌ Respuesta no es JSON válido${NC}"
fi

# Prueba 3: Verificar campos requeridos
echo ""
echo "3️⃣  Prueba de campos requeridos:"
REQUIRED_FIELDS=("timestamp" "hardware" "system" "cpu" "memory" "disk" "processes")
ALL_PRESENT=true

for field in "${REQUIRED_FIELDS[@]}"; do
    if echo "$RESPONSE" | grep -q "\"$field\""; then
        echo -e "${GREEN}   ✅ Campo '$field' presente${NC}"
    else
        echo -e "${RED}   ❌ Campo '$field' faltante${NC}"
        ALL_PRESENT=false
    fi
done

# Prueba 4: Cliente personalizado
echo ""
echo "4️⃣  Prueba con cliente personalizado:"
if ./client_test > /dev/null 2>&1; then
    echo -e "${GREEN}   ✅ Cliente personalizado funciona correctamente${NC}"
else
    echo -e "${RED}   ❌ Error en cliente personalizado${NC}"
fi

# Prueba 5: Múltiples conexiones concurrentes
echo ""
echo "5️⃣  Prueba de múltiples conexiones concurrentes:"
for i in {1..5}; do
    curl -s http://localhost:8080 > /dev/null &
done
wait
echo -e "${GREEN}   ✅ Múltiples conexiones manejadas correctamente${NC}"

# Mostrar respuesta de ejemplo
echo ""
echo -e "${YELLOW}📄 Ejemplo de respuesta del servidor:${NC}"
echo "----------------------------------------"
curl -s http://localhost:8080 | python -m json.tool 2>/dev/null | head -20
echo "..."
echo ""

# Resumen de pruebas
echo -e "${YELLOW}📊 Resumen de pruebas:${NC}"
echo "====================="
if $ALL_PRESENT; then
    echo -e "${GREEN}🎉 Todas las pruebas pasaron exitosamente${NC}"
    echo ""
    echo "El microservicio está funcionando correctamente y proporciona:"
    echo "• Información de CPU (modelo y uso)"
    echo "• Información de memoria (total, usada, libre)"
    echo "• Información de disco (total, usado, libre)"
    echo "• Número de procesos activos"
    echo "• IP del sistema y estado de red"
    echo "• Timestamp de la consulta"
else
    echo -e "${RED}⚠️  Algunas pruebas fallaron${NC}"
fi

# Limpiar (matar servidor si lo iniciamos nosotros)
if [ ! -z "$SERVER_PID" ]; then
    echo ""
    echo -e "${YELLOW}🛑 Deteniendo servidor de pruebas...${NC}"
    kill $SERVER_PID 2>/dev/null
    wait $SERVER_PID 2>/dev/null
    echo -e "${GREEN}✅ Servidor detenido${NC}"
fi

echo ""
echo -e "${YELLOW}🎓 Para usar el microservicio:${NC}"
echo "1. Ejecuta: make run"
echo "2. En otra terminal: curl http://localhost:8080"
echo "3. O abre en navegador: http://localhost:8080"
echo ""
echo -e "${GREEN}¡Happy coding! 🚀${NC}"
