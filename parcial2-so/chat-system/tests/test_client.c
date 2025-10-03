#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "common.h"
#include "message_queue.h"

void test_send_receive_message() {
    int msgid;
    struct message msg;

    msgid = create_message_queue(IPC_PRIVATE);
    if (msgid == -1) {
        perror("Failed to create message queue");
        exit(EXIT_FAILURE);
    }

    strcpy(msg.text, "Hello, World!");
    msg.type = 1;

    if (send_message(msgid, &msg) == -1) {
        perror("Failed to send message");
        exit(EXIT_FAILURE);
    }

    struct message received_msg;
    if (receive_message(msgid, &received_msg, 1) == -1) {
        perror("Failed to receive message");
        exit(EXIT_FAILURE);
    }

    printf("Received message: %s\n", received_msg.text);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("Failed to delete message queue");
        exit(EXIT_FAILURE);
    }
}

int main() {
    test_send_receive_message();
    return 0;
}