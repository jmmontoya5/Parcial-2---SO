#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

// Función para crear una cola de mensajes
int create_message_queue(key_t key);

// Función para enviar un mensaje a la cola
int send_message(int msgid, const message_t *msg);

// Función para recibir un mensaje de la cola
int receive_message(int msgid, message_t *msg, long msg_type);

// Función para eliminar la cola de mensajes
void delete_message_queue(int msgid);

#endif // MESSAGE_QUEUE_H