#!/bin/bash
echo "Limpiando colas de mensajes del sistema de chat..."

# Obtener ID de usuario actual
USER_ID=$(id -u)

# Limpiar colas de mensajes del usuario actual
for queue in $(ipcs -q | grep $USER_ID | awk '{print $2}')
do
    echo "Eliminando cola: $queue"
    ipcrm -q $queue
done

# Limpiar archivos de log si se desea
read -p "¿Eliminar archivos de log? (y/N): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
    rm -f sala_*.log
    echo "Archivos de log eliminados"
fi

echo "Limpieza completada"