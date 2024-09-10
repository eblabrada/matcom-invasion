#!/bin/bash

# Directorio donde se encuentran los archivos .o
DIRECTORY="Engine"

# Verificar si el directorio existe
if [ -d "$DIRECTORY" ]; then
    # Encontrar y eliminar todos los archivos .o en el directorio
    find "$DIRECTORY" -type f -name "*.o" -exec rm -f {} \;
    echo "Todos los archivos .o en el directorio $DIRECTORY han sido eliminados."
else
    echo "El directorio $DIRECTORY no existe."
fi