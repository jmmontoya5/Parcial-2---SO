#!/bin/bash
echo "Compilando sistema de chat..."

# Crear directorio build si no existe
mkdir -p build

# Compilar servidor
echo "Compilando servidor..."
gcc -Wall -Wextra -pthread -std=c99 -Iinclude src/servidor.c -o build/servidor

if [ $? -eq 0 ]; then
    echo "✓ Servidor compilado exitosamente"
else
    echo "✗ Error compilando servidor"
    exit 1
fi

# Compilar cliente
echo "Compilando cliente..."
gcc -Wall -Wextra -pthread -std=c99 -Iinclude src/cliente.c -o build/cliente

if [ $? -eq 0 ]; then
    echo "✓ Cliente compilado exitosamente"
else
    echo "✗ Error compilando cliente"
    exit 1
fi

echo "Compilación completada. Ejecutables en directorio build/"