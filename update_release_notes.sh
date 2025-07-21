#!/bin/bash

# Script para actualizar RELEASE_NOTES.md con una nueva versión
# Uso: ./update_release_notes.sh <version> <description>

set -e

VERSION=${1}
DESCRIPTION=${2:-"Nuevas características y mejoras"}

if [ -z "$VERSION" ]; then
    echo "❌ Error: Debes proporcionar una versión"
    echo "Uso: $0 <version> [description]"
    echo "Ejemplo: $0 v1.2.0 'Análisis de red avanzado'"
    exit 1
fi

# Obtener fecha actual
CURRENT_DATE=$(date "+%d de %B de %Y")

# Crear backup del archivo actual
cp RELEASE_NOTES.md RELEASE_NOTES.md.backup

echo "📝 Actualizando RELEASE_NOTES.md para $VERSION..."

# Crear entrada temporal para la nueva versión
cat > new_version_entry.tmp << EOF

---

## 🚀 $VERSION - $DESCRIPTION
**Fecha**: $CURRENT_DATE

### 🔥 Nuevas Funcionalidades
<!-- Agregar aquí las nuevas funcionalidades -->

### 🔧 Mejoras
<!-- Agregar aquí las mejoras -->

### 🐛 Correcciones
<!-- Agregar aquí las correcciones -->

### 🔧 Cambios Técnicos
<!-- Agregar aquí los cambios técnicos -->

EOF

# Insertar la nueva versión después de la línea "---" (después del header)
awk '
    /^---$/ && !inserted {
        print
        while ((getline line < "new_version_entry.tmp") > 0) {
            print line
        }
        close("new_version_entry.tmp")
        inserted = 1
        next
    }
    { print }
' RELEASE_NOTES.md.backup > RELEASE_NOTES.md

# Limpiar archivos temporales
rm new_version_entry.tmp RELEASE_NOTES.md.backup

echo "✅ RELEASE_NOTES.md actualizado exitosamente"
echo ""
echo "📋 Próximos pasos:"
echo "1. Edita RELEASE_NOTES.md para agregar los detalles específicos"
echo "2. Haz commit de los cambios: git add RELEASE_NOTES.md && git commit -m 'docs: Update release notes for $VERSION'"
echo "3. Crea la nueva release: gh release create $VERSION --notes-file RELEASE_NOTES.md"
echo ""
echo "💡 Tip: El archivo ya tiene la estructura básica, solo completa las secciones relevantes"
