#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "../include/chat_protocol.h"

// Variables globales del cliente
int cola_global, cola_cliente;
char nombre_usuario[MAX_NOMBRE];
char sala_actual[MAX_NOMBRE];
int en_sala = 0;
pthread_t hilo_receptor;

// Función para crear cola privada del cliente
int crear_cola_privada() {
    key_t key = ftok("/tmp", getpid());
    if (key == -1) {
        perror("Error generando key para cola privada");
        return -1;
    }
    
    int cola = msgget(key, IPC_CREAT | 0666);
    if (cola == -1) {
        perror("Error creando cola privada");
        return -1;
    }
    
    return cola;
}

// Función del hilo receptor para mensajes
void* hilo_receptor_func(void* arg) {
    (void)arg; // Suprimir warning de parámetro no usado
    struct mensaje msg;
    
    while (1) {
        if (msgrcv(cola_cliente, &msg, sizeof(struct mensaje) - sizeof(long), 0, 0) == -1) {
            if (errno == EINTR) continue;
            perror("Error recibiendo mensaje en cliente");
            continue;
        }
        
        switch (msg.mtype) {
            case MT_JOIN_ACK:
                printf("\n✓ Te has unido a la sala: %s\n", msg.sala);
                strcpy(sala_actual, msg.sala);
                en_sala = 1;
                break;
            case MT_MSG:
                printf("\n%s: %s\n", msg.remitente, msg.texto);
                break;
            case MT_LIST_ACK:
                printf("\n%s\n", msg.texto);
                break;
            case MT_USERS_ACK:
                printf("\n%s\n", msg.texto);
                break;
            case MT_LEAVE_ACK:
                printf("\n✓ %s\n", msg.texto);
                en_sala = 0;
                memset(sala_actual, 0, sizeof(sala_actual));
                break;
            case MT_ERROR:
                printf("\n✗ Error: %s\n", msg.texto);
                break;
            default:
                printf("\nMensaje desconocido recibido\n");
        }
        printf("> ");
        fflush(stdout);
    }
    
    return NULL;
}

// Función para conectar al servidor
int conectar_servidor() {
    cola_global = msgget(KEY_GLOBAL, 0);
    if (cola_global == -1) {
        printf("Error: No se puede conectar al servidor. ¿Está ejecutándose?\n");
        return -1;
    }
    
    cola_cliente = crear_cola_privada();
    if (cola_cliente == -1) {
        return -1;
    }
    
    printf("Conectado al servidor (cola privada: %d)\n", cola_cliente);
    return 0;
}

// Función para enviar comando JOIN
void comando_join(const char* nombre_sala) {
    struct mensaje msg;
    memset(&msg, 0, sizeof(msg));
    
    msg.mtype = MT_JOIN;
    strcpy(msg.remitente, nombre_usuario);
    strcpy(msg.sala, nombre_sala);
    msg.client_queue_id = cola_cliente;
    
    if (msgsnd(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
        perror("Error enviando JOIN");
    } else {
        printf("Intentando unirse a la sala: %s...\n", nombre_sala);
    }
}

// Función para enviar comando LIST
void comando_list() {
    struct mensaje msg;
    memset(&msg, 0, sizeof(msg));
    
    msg.mtype = MT_LIST;
    strcpy(msg.remitente, nombre_usuario);
    msg.client_queue_id = cola_cliente;
    
    if (msgsnd(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
        perror("Error enviando LIST");
    }
}

// Función para enviar comando USERS
void comando_users() {
    if (!en_sala) {
        printf("No estás en ninguna sala. Usa 'join <sala>' primero.\n");
        return;
    }
    
    struct mensaje msg;
    memset(&msg, 0, sizeof(msg));
    
    msg.mtype = MT_USERS;
    strcpy(msg.remitente, nombre_usuario);
    strcpy(msg.sala, sala_actual);
    msg.client_queue_id = cola_cliente;
    
    if (msgsnd(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
        perror("Error enviando USERS");
    }
}

// Función para enviar comando LEAVE
void comando_leave() {
    if (!en_sala) {
        printf("No estás en ninguna sala.\n");
        return;
    }
    
    struct mensaje msg;
    memset(&msg, 0, sizeof(msg));
    
    msg.mtype = MT_LEAVE;
    strcpy(msg.remitente, nombre_usuario);
    strcpy(msg.sala, sala_actual);
    msg.client_queue_id = cola_cliente;
    
    if (msgsnd(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
        perror("Error enviando LEAVE");
    }
}

// Función para enviar mensaje de chat
void enviar_mensaje(const char* texto) {
    if (!en_sala) {
        printf("No estás en ninguna sala. Usa 'join <sala>' primero.\n");
        return;
    }
    
    if (strlen(texto) == 0) return;
    
    struct mensaje msg;
    memset(&msg, 0, sizeof(msg));
    
    msg.mtype = MT_MSG;
    strcpy(msg.remitente, nombre_usuario);
    strcpy(msg.texto, texto);
    strcpy(msg.sala, sala_actual);
    msg.client_queue_id = cola_cliente;
    
    if (msgsnd(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
        perror("Error enviando mensaje");
    }
}

// Función para procesar comando de usuario
void procesar_comando(char* entrada) {
    // Remover salto de línea
    entrada[strcspn(entrada, "\n")] = 0;
    
    if (strlen(entrada) == 0) return;
    
    if (strncmp(entrada, "join ", 5) == 0) {
        char* nombre_sala = entrada + 5;
        if (strlen(nombre_sala) > 0) {
            comando_join(nombre_sala);
        } else {
            printf("Uso: join <nombre_sala>\n");
        }
    }
    else if (strcmp(entrada, "/list") == 0) {
        comando_list();
    }
    else if (strcmp(entrada, "/users") == 0) {
        comando_users();
    }
    else if (strcmp(entrada, "leave") == 0) {
        comando_leave();
    }
    else if (strcmp(entrada, "/help") == 0) {
        printf("\nComandos disponibles:\n");
        printf("  join <sala>  - Unirse a una sala\n");
        printf("  leave        - Salir de la sala actual\n");
        printf("  /list        - Ver salas disponibles\n");
        printf("  /users       - Ver usuarios en la sala actual\n");
        printf("  /help        - Mostrar esta ayuda\n");
        printf("  quit         - Salir del cliente\n");
        printf("\nTambién puedes escribir mensajes directamente para enviar a la sala.\n\n");
    }
    else if (strcmp(entrada, "quit") == 0) {
        printf("Saliendo...\n");
        exit(0);
    }
    else {
        // Es un mensaje de chat
        enviar_mensaje(entrada);
    }
}

// Signal handler para limpieza del cliente
void signal_handler(int sig) {
    (void)sig; // Suprimir warning de parámetro no usado
    printf("\nDesconectando...\n");
    if (cola_cliente != -1) {
        msgctl(cola_cliente, IPC_RMID, NULL);
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_usuario>\n", argv[0]);
        printf("Ejemplo: %s Maria\n", argv[0]);
        return 1;
    }
    
    strcpy(nombre_usuario, argv[1]);
    
    // Configurar signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== Cliente de Chat ===\n");
    printf("Bienvenido, %s!\n", nombre_usuario);
    
    if (conectar_servidor() == -1) {
        return 1;
    }
    
    // Crear hilo receptor
    if (pthread_create(&hilo_receptor, NULL, hilo_receptor_func, NULL) != 0) {
        perror("Error creando hilo receptor");
        return 1;
    }
    
    // Mostrar salas disponibles al inicio
    printf("\nObteniendo salas disponibles...\n");
    comando_list();
    
    printf("\nEscribe '/help' para ver los comandos disponibles.\n");
    printf("Usa 'join <nombre_sala>' para unirte a una sala.\n\n");
    
    char entrada[MAX_TEXTO];
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
            break;
        }
        
        procesar_comando(entrada);
    }
    
    return 0;
}