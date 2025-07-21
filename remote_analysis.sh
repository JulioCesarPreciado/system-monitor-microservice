#!/bin/bash

# Script para análisis remoto de servidores usando el microservicio
# Uso: ./remote_analysis.sh [servidor] [usuario] [puerto_ssh]

set -e

SERVER=${1:-"localhost"}
USER=${2:-$(whoami)}
SSH_PORT=${3:-"22"}
REMOTE_PATH=${4:-"./system_monitor"}

echo "🔍 ANÁLISIS REMOTO DE SERVIDOR"
echo "════════════════════════════════"
echo "Servidor: $SERVER"
echo "Usuario: $USER"
echo "Puerto SSH: $SSH_PORT"
echo "Ejecutable remoto: $REMOTE_PATH"
echo ""

if [ "$SERVER" = "localhost" ]; then
    echo "📊 ANÁLISIS LOCAL:"
    echo "─────────────────"
    ./system_monitor --processes
else
    echo "📡 Conectando a servidor remoto..."
    
    # Verificar conectividad
    if ! ssh -p "$SSH_PORT" -o ConnectTimeout=5 "$USER@$SERVER" "echo 'Conexión exitosa'" 2>/dev/null; then
        echo "❌ Error: No se puede conectar al servidor $SERVER"
        echo "Verifica:"
        echo "  • Conectividad de red"
        echo "  • Credenciales SSH"
        echo "  • Puerto SSH ($SSH_PORT)"
        exit 1
    fi
    
    echo "✅ Conexión establecida"
    echo ""
    echo "📊 EJECUTANDO ANÁLISIS REMOTO:"
    echo "──────────────────────────────"
    
    # Ejecutar análisis remoto
    ssh -p "$SSH_PORT" "$USER@$SERVER" "$REMOTE_PATH --processes" || {
        echo ""
        echo "❌ Error al ejecutar análisis remoto"
        echo "Posibles soluciones:"
        echo "  • Subir el ejecutable: scp ./system_monitor $USER@$SERVER:$REMOTE_PATH"
        echo "  • Compilar remotamente: ssh $USER@$SERVER 'cd proyecto && make'"
        echo "  • Verificar permisos: ssh $USER@$SERVER 'chmod +x $REMOTE_PATH'"
        exit 1
    }
fi

echo ""
echo "✅ Análisis completado"

# Ejemplo de uso con API HTTP (si el servidor está corriendo)
if [ "$SERVER" != "localhost" ]; then
    echo ""
    echo "💡 Para análisis HTTP continuo, inicia el servidor remoto:"
    echo "   ssh $USER@$SERVER '$REMOTE_PATH &'"
    echo "   curl http://$SERVER:8080/processes/top"
fi
