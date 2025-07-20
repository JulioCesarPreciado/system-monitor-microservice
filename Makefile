# Makefile para Sistema de Monitoreo - Microservicio en C
# Estructura modular con detección automática de plataforma

# Configuración del compilador
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
DEBUG_FLAGS = -g -DDEBUG
INCLUDES = -Iinclude

# Directorios
SRC_DIR = src
UTILS_DIR = utils
INCLUDE_DIR = include
TARGET_DIR = .

# Archivos fuente
MAIN_SRC = main.c
SRC_FILES = $(SRC_DIR)/system_info.c $(SRC_DIR)/server.c
UTILS_FILES = $(UTILS_DIR)/platform.c

# Nombre del ejecutable
TARGET = system_monitor
CLIENT_TARGET = client_test

# Detectar sistema operativo para flags específicos
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    PLATFORM_FLAGS = -DMACOS
    LDFLAGS = 
else ifeq ($(UNAME_S),Linux)
    PLATFORM_FLAGS = -DLINUX
    LDFLAGS = 
else
    PLATFORM_FLAGS = -DUNKNOWN_OS
    LDFLAGS = 
endif

# Regla principal: compilar todo
all: info $(TARGET) $(CLIENT_TARGET)

# Mostrar información del sistema
info:
	@echo "🔧 Configuración de Compilación"
	@echo "═════════════════════════════════"
	@echo "Sistema: $(UNAME_S)"
	@echo "Compilador: $(CC)"
	@echo "Flags: $(CFLAGS) $(PLATFORM_FLAGS)"
	@echo "Target: $(TARGET)"
	@echo ""

# Compilar el programa principal
$(TARGET): $(MAIN_SRC) $(SRC_FILES) $(UTILS_FILES)
	@echo "🔨 Compilando microservicio principal..."
	$(CC) $(CFLAGS) $(PLATFORM_FLAGS) $(INCLUDES) -o $(TARGET_DIR)/$(TARGET) \
		$(MAIN_SRC) $(SRC_FILES) $(UTILS_FILES) $(LDFLAGS)
	@echo "✅ $(TARGET) compilado exitosamente"

# Compilar el cliente de prueba
$(CLIENT_TARGET): client_test.c
	@echo "🔨 Compilando cliente de prueba..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET_DIR)/$(CLIENT_TARGET) client_test.c
	@echo "✅ $(CLIENT_TARGET) compilado exitosamente"

# Compilación en modo debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)
	@echo "🐛 Compilado en modo debug"

# Ejecutar el servidor
run: $(TARGET)
	@echo "🚀 Iniciando servidor..."
	./$(TARGET)

# Ejecutar con información de plataforma
run-info: $(TARGET)
	@echo "🖥️  Mostrando información de plataforma..."
	./$(TARGET) --platform
	@echo ""
	@echo "🚀 Iniciando servidor..."
	./$(TARGET)

# Probar el servidor (compilar y hacer pruebas básicas)
test: $(TARGET) $(CLIENT_TARGET)
	@echo "🧪 Ejecutando pruebas básicas..."
	@echo "1️⃣  Verificando compilación: ✅"
	@echo "2️⃣  Verificando argumentos de ayuda:"
	./$(TARGET) --help | head -5
	@echo "3️⃣  Verificando información de versión:"
	./$(TARGET) --version
	@echo "4️⃣  Verificando información de plataforma:"
	./$(TARGET) --platform | head -10
	@echo ""
	@echo "✅ Pruebas básicas completadas"
	@echo ""
	@echo "Para probar el servidor completo:"
	@echo "  1. Terminal 1: make run"
	@echo "  2. Terminal 2: make test-client"
	@echo "  3. O: curl http://localhost:8080"

# Probar con cliente personalizado
test-client: $(CLIENT_TARGET)
	@echo "🧪 Probando con cliente personalizado..."
	./$(CLIENT_TARGET)

# Script de pruebas completo
test-full: $(TARGET)
	@if [ -f test_microservice.sh ]; then \
		echo "🧪 Ejecutando script completo de pruebas..."; \
		./test_microservice.sh; \
	else \
		echo "❌ Script test_microservice.sh no encontrado"; \
		echo "Ejecutando pruebas básicas..."; \
		$(MAKE) test; \
	fi

# Limpiar archivos compilados
clean:
	@echo "🧹 Limpiando archivos compilados..."
	rm -f $(TARGET_DIR)/$(TARGET)
	rm -f $(TARGET_DIR)/$(CLIENT_TARGET)
	@echo "✅ Limpieza completada"

# Instalar (copiar a directorio del sistema)
install: $(TARGET)
	@echo "📦 Instalando $(TARGET)..."
	sudo cp $(TARGET_DIR)/$(TARGET) /usr/local/bin/
	@echo "✅ $(TARGET) instalado en /usr/local/bin/"
	@echo "Ahora puedes ejecutar '$(TARGET)' desde cualquier lugar"

# Desinstalar
uninstall:
	@echo "🗑️  Desinstalando $(TARGET)..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "✅ $(TARGET) desinstalado"

# Crear paquete de distribución
package: clean $(TARGET)
	@echo "📦 Creando paquete de distribución..."
	@PACKAGE_NAME="system_monitor_$(UNAME_S)_$(shell date +%Y%m%d)" && \
	mkdir -p $$PACKAGE_NAME && \
	cp $(TARGET) $$PACKAGE_NAME/ && \
	cp README.md $$PACKAGE_NAME/ 2>/dev/null || true && \
	cp PROYECTO_COMPLETADO.md $$PACKAGE_NAME/ 2>/dev/null || true && \
	tar -czf $$PACKAGE_NAME.tar.gz $$PACKAGE_NAME && \
	rm -rf $$PACKAGE_NAME && \
	echo "✅ Paquete creado: $$PACKAGE_NAME.tar.gz"

# Análisis estático del código (si está disponible)
analyze:
	@echo "🔍 Analizando código..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c99 $(MAIN_SRC) $(SRC_FILES) $(UTILS_FILES); \
	else \
		echo "⚠️  cppcheck no disponible, saltando análisis estático"; \
	fi

# Verificar memory leaks (si valgrind está disponible)
memcheck: debug
	@echo "🔍 Verificando memory leaks..."
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "Ejecuta: valgrind --leak-check=full ./$(TARGET)"; \
	else \
		echo "⚠️  valgrind no disponible"; \
	fi

# Mostrar estadísticas del código
stats:
	@echo "📊 Estadísticas del código:"
	@echo "════════════════════════════"
	@echo -n "Archivos fuente: "
	@find . -name "*.c" -o -name "*.h" | wc -l
	@echo -n "Líneas de código total: "
	@find . -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print $$1}'
	@echo -n "Líneas en main.c: "
	@wc -l < $(MAIN_SRC)
	@echo -n "Líneas en archivos de src/: "
	@find $(SRC_DIR) -name "*.c" | xargs wc -l | tail -1 | awk '{print $$1}' 2>/dev/null || echo "0"
	@echo -n "Líneas en archivos de utils/: "
	@find $(UTILS_DIR) -name "*.c" | xargs wc -l | tail -1 | awk '{print $$1}' 2>/dev/null || echo "0"

# Ayuda
help:
	@echo "🆘 Sistema de Monitoreo - Comandos Disponibles"
	@echo "═════════════════════════════════════════════"
	@echo "Compilación:"
	@echo "  make            - Compilar todo (release)"
	@echo "  make debug      - Compilar en modo debug"
	@echo "  make clean      - Limpiar archivos compilados"
	@echo ""
	@echo "Ejecución:"
	@echo "  make run        - Ejecutar el servidor"
	@echo "  make run-info   - Ejecutar con info de plataforma"
	@echo ""
	@echo "Pruebas:"
	@echo "  make test       - Pruebas básicas"
	@echo "  make test-client - Probar cliente personalizado"
	@echo "  make test-full  - Ejecutar todas las pruebas"
	@echo ""
	@echo "Utilidades:"
	@echo "  make install    - Instalar en el sistema"
	@echo "  make uninstall  - Desinstalar del sistema"
	@echo "  make package    - Crear paquete de distribución"
	@echo "  make stats      - Mostrar estadísticas del código"
	@echo "  make analyze    - Análisis estático (requiere cppcheck)"
	@echo "  make memcheck   - Verificar memory leaks (requiere valgrind)"
	@echo ""
	@echo "Información:"
	@echo "  make help       - Mostrar esta ayuda"
	@echo "  ./$(TARGET) --help    - Ayuda del programa"
	@echo "  ./$(TARGET) --version - Versión del programa"

# Declarar targets que no son archivos
.PHONY: all info run run-info test test-client test-full clean install uninstall package analyze memcheck stats help debug
