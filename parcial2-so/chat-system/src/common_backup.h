#ifndef COMMON_H
#define COMMON_H

#include <sys/types.h>

// Definición de constantes para tipos de mensajes
#define MSG_TYPE_TEXT 1
#define MSG_TYPE_COMMAND 2

// Estructura de mensaje
typedef struct {
    long msg_type;        // Tipo de mensaje
    pid_t sender_pid;     // PID del remitente
    char text[256];       // Contenido del mensaje
} Message;

// Estructura para comandos
typedef struct {
    long msg_type;        // Tipo de mensaje
    pid_t sender_pid;     // PID del remitente
    char command[256];    // Comando enviado
} Command;

#endif // COMMON_H