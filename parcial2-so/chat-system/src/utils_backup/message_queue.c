#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "message_queue.h"

int create_message_queue(key_t key) {
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    return msgid;
}

int send_message(int msgid, const char *message, long message_type) {
    struct message_buf {
        long mtype;
        char mtext[256];
    } msg;

    msg.mtype = message_type;
    strncpy(msg.mtext, message, sizeof(msg.mtext) - 1);
    msg.mtext[sizeof(msg.mtext) - 1] = '\0';

    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        return -1;
    }
    return 0;
}

int receive_message(int msgid, long message_type, char *buffer, size_t buffer_size) {
    struct message_buf {
        long mtype;
        char mtext[256];
    } msg;

    if (msgrcv(msgid, &msg, sizeof(msg.mtext), message_type, 0) == -1) {
        perror("msgrcv");
        return -1;
    }
    strncpy(buffer, msg.mtext, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
    return 0;
}

void delete_message_queue(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }
}