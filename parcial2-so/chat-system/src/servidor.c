#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "../include/chat_protocol.h"

// Variables globales del servidor
int cola_global;
struct sala salas[MAX_SALAS];
int num_salas = 0;

// Función para inicializar salas por defecto
void inicializar_salas() {
    strcpy(salas[0].nombre, "General");
    salas[0].num_usuarios = 0;
    salas[0].activa = 1;
    
    strcpy(salas[1].nombre, "Deportes");
    salas[1].num_usuarios = 0;
    salas[1].activa = 1;
    
    num_salas = 2;
}

// Función para encontrar sala por nombre
int encontrar_sala(const char* nombre_sala) {
    for (int i = 0; i < num_salas; i++) {
        if (strcmp(salas[i].nombre, nombre_sala) == 0 && salas[i].activa) {
            return i;
        }
    }
    return -1;
}

// Función para agregar usuario a sala
int agregar_usuario_sala(int indice_sala, const char* nombre, int queue_id, pid_t pid) {
    if (indice_sala < 0 || indice_sala >= num_salas) return -1;
    
    struct sala* s = &salas[indice_sala];
    
    // Verificar si el usuario ya está en la sala
    for (int i = 0; i < s->num_usuarios; i++) {
        if (strcmp(s->usuarios[i].nombre, nombre) == 0) {
            return -2; // Usuario ya en sala
        }
    }
    
    // Verificar si hay espacio
    if (s->num_usuarios >= MAX_USUARIOS_POR_SALA) {
        return -3; // Sala llena
    }
    
    // Agregar usuario
    strcpy(s->usuarios[s->num_usuarios].nombre, nombre);
    s->usuarios[s->num_usuarios].queue_id = queue_id;
    s->usuarios[s->num_usuarios].pid = pid;
    s->num_usuarios++;
    
    printf("Usuario %s agregado a sala %s (cola: %d)\n", nombre, s->nombre, queue_id);
    return 0;
}

// Función para remover usuario de sala
int remover_usuario_sala(int indice_sala, const char* nombre) {
    if (indice_sala < 0 || indice_sala >= num_salas) return -1;
    
    struct sala* s = &salas[indice_sala];
    
    for (int i = 0; i < s->num_usuarios; i++) {
        if (strcmp(s->usuarios[i].nombre, nombre) == 0) {
            // Mover usuarios hacia arriba
            for (int j = i; j < s->num_usuarios - 1; j++) {
                s->usuarios[j] = s->usuarios[j + 1];
            }
            s->num_usuarios--;
            printf("Usuario %s removido de sala %s\n", nombre, s->nombre);
            return 0;
        }
    }
    return -1; // Usuario no encontrado
}

// Función para broadcast de mensaje en sala
void broadcast_mensaje(int indice_sala, struct mensaje* msg) {
    if (indice_sala < 0 || indice_sala >= num_salas) return;
    
    struct sala* s = &salas[indice_sala];
    
    for (int i = 0; i < s->num_usuarios; i++) {
        // No enviar mensaje al remitente
        if (strcmp(s->usuarios[i].nombre, msg->remitente) == 0) continue;
        
        struct mensaje msg_broadcast = *msg;
        msg_broadcast.mtype = MT_MSG;
        
        if (msgsnd(s->usuarios[i].queue_id, &msg_broadcast, 
                   sizeof(struct mensaje) - sizeof(long), IPC_NOWAIT) == -1) {
            printf("Error enviando mensaje a %s: %s\n", 
                   s->usuarios[i].nombre, strerror(errno));
        }
    }
}

// Función para manejar JOIN
void manejar_join(struct mensaje* msg) {
    int indice_sala = encontrar_sala(msg->sala);
    struct mensaje respuesta;
    memset(&respuesta, 0, sizeof(respuesta));
    
    if (indice_sala == -1) {
        // Crear nueva sala si hay espacio
        if (num_salas < MAX_SALAS) {
            strcpy(salas[num_salas].nombre, msg->sala);
            salas[num_salas].num_usuarios = 0;
            salas[num_salas].activa = 1;
            indice_sala = num_salas;
            num_salas++;
            printf("Nueva sala creada: %s\n", msg->sala);
        } else {
            respuesta.mtype = MT_ERROR;
            respuesta.error_code = -1;
            strcpy(respuesta.texto, "No se pueden crear más salas");
            msgsnd(msg->client_queue_id, &respuesta, sizeof(struct mensaje) - sizeof(long), 0);
            return;
        }
    }
    
    int resultado = agregar_usuario_sala(indice_sala, msg->remitente, 
                                        msg->client_queue_id, getpid());
    
    if (resultado == 0) {
        respuesta.mtype = MT_JOIN_ACK;
        strcpy(respuesta.sala, msg->sala);
        strcpy(respuesta.texto, "Unido exitosamente");
    } else {
        respuesta.mtype = MT_ERROR;
        respuesta.error_code = resultado;
        if (resultado == -2) {
            strcpy(respuesta.texto, "Ya estás en esta sala");
        } else if (resultado == -3) {
            strcpy(respuesta.texto, "Sala llena");
        }
    }
    
    msgsnd(msg->client_queue_id, &respuesta, sizeof(struct mensaje) - sizeof(long), 0);
}

// Función para manejar LIST
void manejar_list(struct mensaje* msg) {
    struct mensaje respuesta;
    memset(&respuesta, 0, sizeof(respuesta));
    respuesta.mtype = MT_LIST_ACK;
    
    strcpy(respuesta.texto, "Salas disponibles: ");
    for (int i = 0; i < num_salas; i++) {
        if (salas[i].activa) {
            strcat(respuesta.texto, salas[i].nombre);
            if (i < num_salas - 1) strcat(respuesta.texto, ", ");
        }
    }
    
    msgsnd(msg->client_queue_id, &respuesta, sizeof(struct mensaje) - sizeof(long), 0);
}

// Función para manejar USERS
void manejar_users(struct mensaje* msg) {
    int indice_sala = encontrar_sala(msg->sala);
    struct mensaje respuesta;
    memset(&respuesta, 0, sizeof(respuesta));
    
    if (indice_sala == -1) {
        respuesta.mtype = MT_ERROR;
        strcpy(respuesta.texto, "Sala no encontrada");
    } else {
        respuesta.mtype = MT_USERS_ACK;
        sprintf(respuesta.texto, "Usuarios en %s: ", msg->sala);
        
        for (int i = 0; i < salas[indice_sala].num_usuarios; i++) {
            strcat(respuesta.texto, salas[indice_sala].usuarios[i].nombre);
            if (i < salas[indice_sala].num_usuarios - 1) {
                strcat(respuesta.texto, ", ");
            }
        }
    }
    
    msgsnd(msg->client_queue_id, &respuesta, sizeof(struct mensaje) - sizeof(long), 0);
}

// Función para manejar LEAVE
void manejar_leave(struct mensaje* msg) {
    int indice_sala = encontrar_sala(msg->sala);
    struct mensaje respuesta;
    memset(&respuesta, 0, sizeof(respuesta));
    
    if (indice_sala == -1) {
        respuesta.mtype = MT_ERROR;
        strcpy(respuesta.texto, "Sala no encontrada");
    } else {
        int resultado = remover_usuario_sala(indice_sala, msg->remitente);
        if (resultado == 0) {
            respuesta.mtype = MT_LEAVE_ACK;
            strcpy(respuesta.texto, "Has salido de la sala");
        } else {
            respuesta.mtype = MT_ERROR;
            strcpy(respuesta.texto, "No estás en esta sala");
        }
    }
    
    msgsnd(msg->client_queue_id, &respuesta, sizeof(struct mensaje) - sizeof(long), 0);
}

// Función para guardar mensaje en log (opcional)
void guardar_en_log(const char* sala, const char* remitente, const char* texto) {
    char filename[100];
    sprintf(filename, "sala_%s.log", sala);
    
    FILE* file = fopen(filename, "a");
    if (file) {
        time_t now = time(NULL);
        char* timestr = ctime(&now);
        timestr[strlen(timestr) - 1] = '\0'; // Remover \n
        
        fprintf(file, "[%s] %s: %s\n", timestr, remitente, texto);
        fclose(file);
    }
}

// Signal handler para limpieza
void signal_handler(int sig) {
    (void)sig; // Suprimir warning
    printf("\nCerrando servidor...\n");
    if (msgctl(cola_global, IPC_RMID, NULL) == -1) {
        perror("Error eliminando cola global");
    }
    exit(0);
}

int main() {
    printf("Iniciando servidor de chat...\n");
    
    // Configurar signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Crear cola global
    cola_global = msgget(KEY_GLOBAL, IPC_CREAT | 0666);
    if (cola_global == -1) {
        perror("Error creando cola global");
        exit(1);
    }
    
    printf("Cola global creada (ID: %d)\n", cola_global);
    
    // Inicializar salas
    inicializar_salas();
    printf("Salas inicializadas: General, Deportes\n");
    printf("Servidor de chat iniciado. Esperando clientes...\n");
    
    struct mensaje msg;
    
    while (1) {
        // Recibir mensaje de cualquier tipo
        if (msgrcv(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0, 0) == -1) {
            if (errno == EINTR) continue;
            perror("Error recibiendo mensaje");
            continue;
        }
        
        printf("Mensaje recibido tipo: %ld de %s\n", msg.mtype, msg.remitente);
        
        switch (msg.mtype) {
            case MT_JOIN:
                manejar_join(&msg);
                break;
            case MT_MSG:
                {
                    int indice_sala = encontrar_sala(msg.sala);
                    if (indice_sala != -1) {
                        broadcast_mensaje(indice_sala, &msg);
                        guardar_en_log(msg.sala, msg.remitente, msg.texto);
                        printf("Mensaje de %s en %s: %s\n", msg.remitente, msg.sala, msg.texto);
                    }
                }
                break;
            case MT_LIST:
                manejar_list(&msg);
                break;
            case MT_USERS:
                manejar_users(&msg);
                break;
            case MT_LEAVE:
                manejar_leave(&msg);
                break;
            default:
                printf("Tipo de mensaje desconocido: %ld\n", msg.mtype);
        }
    }
    
    return 0;
}