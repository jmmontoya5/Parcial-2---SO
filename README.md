# Sistema de Chat con Colas de Mensajes

Este proyecto implementa un sistema de chat multi-sala usando colas de mensajes System V en C para Linux.

## Características

- **Múltiples salas**: Los usuarios pueden unirse a diferentes salas de chat
- **Comunicación en tiempo real**: Mensajes instantáneos entre usuarios de la misma sala
- **Comandos**: Lista de salas, usuarios, unirse/salir de salas
- **Persistencia**: Historial de mensajes guardado en archivos log
- **Manejo robusto de errores**: Validación y limpieza de recursos

## Arquitectura

### Diseño de Colas
- **Cola Global**: Usada para comandos al servidor (JOIN, LEAVE, LIST, etc.)
- **Colas Privadas**: Cada cliente tiene su propia cola para recibir mensajes
- **Broadcast**: El servidor envía mensajes individualmente a cada cola privada

### Tipos de Mensaje
- `MT_JOIN`: Unirse a una sala
- `MT_MSG`: Enviar mensaje de chat
- `MT_LIST`: Listar salas disponibles
- `MT_USERS`: Listar usuarios en sala actual
- `MT_LEAVE`: Salir de sala
- `MT_ERROR`: Mensajes de error

## Compilación

```bash
# Compilar todo
make

# Compilar solo servidor
make build/servidor

# Compilar solo cliente
make build/cliente

# Limpiar archivos compilados
make clean
```

## Ejecución

### 1. Iniciar el Servidor
```bash
# Terminal 1
./build/servidor
```

El servidor creará las salas por defecto: "General" y "Deportes".

### 2. Conectar Clientes
```bash
# Terminal 2
./build/cliente Maria

# Terminal 3  
./build/cliente Juan

# Terminal 4
./build/cliente Camila
```

O usar el Makefile:
```bash
make run-client NAME=Maria
```

## Comandos del Cliente

| Comando | Descripción |
|---------|-------------|
| `join <sala>` | Unirse a una sala (ej: `join General`) |
| `leave` | Salir de la sala actual |
| `/list` | Ver todas las salas disponibles |
| `/users` | Ver usuarios en la sala actual |
| `/help` | Mostrar ayuda |
| `quit` | Salir del cliente |

Para enviar mensajes, simplemente escribe el texto y presiona Enter.

## Ejemplo de Uso

```bash
# En cliente Maria
> /list
Salas disponibles: General, Deportes

> join General
✓ Te has unido a la sala: General

> Hola a todos!

> /users
Usuarios en General: Maria, Juan

# En cliente Juan (recibirá)
Maria: Hola a todos!

> join General
✓ Te has unido a la sala: General

> Hola Maria!

# En cliente Maria (recibirá)
Juan: Hola Maria!
```

## Funcionalidades Implementadas

### Obligatorias ✅
- [x] Colas de mensajes System V
- [x] Servidor central con gestión de salas
- [x] Múltiples clientes con colas privadas
- [x] Comandos: join, leave, /list, /users
- [x] Broadcast de mensajes en sala
- [x] Manejo de errores robusto
- [x] Documentación completa

### Adicionales ✅
- [x] Persistencia en archivos log (`sala_<nombre>.log`)
- [x] Comando /help
- [x] Limpieza automática de recursos
- [x] Makefile completo
- [x] Manejo de señales (SIGINT, SIGTERM)

## Archivos Generados

El sistema genera archivos de log automáticamente:
- `sala_General.log`
- `sala_Deportes.log`
- `sala_<nombre>.log` (para salas creadas dinámicamente)

Formato de log:
```
[Wed Oct  2 15:30:45 2025] Maria: Hola a todos!
[Wed Oct  2 15:31:02 2025] Juan: Hola Maria!
```

## Limpieza del Sistema

Para limpiar colas de mensajes huérfanas:
```bash
# Ejecutar script de limpieza
./scripts/cleanup.sh

# O usar Makefile
make cleanup
```

## Estructura del Proyecto

```
chat-system/
├── src/
│   ├── servidor.c          # Servidor principal
│   ├── cliente.c           # Cliente de chat
│   └── common.h           # Definiciones comunes
├── include/
│   └── chat_protocol.h    # Protocolo y estructuras
├── build/                 # Ejecutables compilados
├── scripts/
│   ├── compile.sh         # Script de compilación
│   └── cleanup.sh         # Script de limpieza
├── docs/
│   └── README.md          # Documentación adicional
├── Makefile              # Sistema de construcción
└── README.md             # Este archivo
```

## Manejo de Errores

El sistema maneja los siguientes errores:

| Error | Descripción | Respuesta |
|-------|-------------|-----------|
| Sala inexistente | Se crea automáticamente | Nueva sala |
| Usuario ya en sala | No se permite duplicados | Error claro |
| Sala llena | Máximo 20 usuarios por sala | Error con código |
| Servidor no disponible | Cliente no puede conectar | Mensaje de error |
| Cola no encontrada | Error en comunicación | Reintento/error |

## Limitaciones

- Máximo 10 salas simultáneas
- Máximo 20 usuarios por sala
- Mensajes de máximo 256 caracteres
- Nombres de usuario de máximo 50 caracteres

## Requisitos del Sistema

- Linux (System V IPC)
- GCC con soporte pthread
- Permisos para crear colas de mensajes

## Solución de Problemas

### Error: "No such file or directory"
```bash
# Verificar que el servidor esté ejecutándose
ps aux | grep servidor

# Limpiar colas huérfanas
make cleanup
```

### Error: "Permission denied"
```bash
# Verificar permisos de IPC
ipcs -q

# Limpiar colas manualmente
ipcrm -q <queue_id>
```

### Cliente no recibe mensajes
1. Verificar que esté en una sala (`join <sala>`)
2. Verificar que el servidor esté ejecutándose
3. Comprobar logs del servidor para errores

## Desarrollo y Contribuciones

Para extender el sistema:

1. Modificar estructuras en `include/chat_protocol.h`
2. Agregar nuevos tipos de mensaje
3. Implementar manejadores en `servidor.c`
4. Agregar comandos en `cliente.c`
5. Actualizar documentación

## Autor

Desarrollado para el curso de Sistemas Operativos.
