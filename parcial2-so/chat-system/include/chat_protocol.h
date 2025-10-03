#ifndef CHAT_PROTOCOL_H
#define CHAT_PROTOCOL_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

// Constantes del sistema
#define MAX_SALAS 10
#define MAX_USUARIOS_POR_SALA 20
#define MAX_TEXTO 256
#define MAX_NOMBRE 50
#define KEY_GLOBAL 1234

// Tipos de mensaje
#define MT_JOIN 1
#define MT_JOIN_ACK 2
#define MT_MSG 3
#define MT_LIST 4
#define MT_LIST_ACK 5
#define MT_USERS 6
#define MT_USERS_ACK 7
#define MT_LEAVE 8
#define MT_LEAVE_ACK 9
#define MT_ERROR 10

// Estructura del mensaje
struct mensaje {
    long mtype;
    char remitente[MAX_NOMBRE];
    char texto[MAX_TEXTO];
    char sala[MAX_NOMBRE];
    int client_queue_id;
    int error_code;
};

// Estructura de usuario
struct usuario {
    char nombre[MAX_NOMBRE];
    int queue_id;
    pid_t pid;
};

// Estructura de sala
struct sala {
    char nombre[MAX_NOMBRE];
    int num_usuarios;
    struct usuario usuarios[MAX_USUARIOS_POR_SALA];
    int activa;
};

// Funciones comunes
void limpiar_recursos();
void signal_handler(int sig);

#endif