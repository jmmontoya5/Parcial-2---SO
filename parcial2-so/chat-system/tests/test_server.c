#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/servidor.h"
#include "../src/utils/message_queue.h"

void test_create_message_queue() {
    int mq_id = create_message_queue();
    assert(mq_id != -1);
    printf("Test create_message_queue passed.\n");
}

void test_send_message() {
    int mq_id = create_message_queue();
    Message msg;
    strcpy(msg.text, "Hello, World!");
    msg.type = 1;

    int result = send_message(mq_id, &msg);
    assert(result != -1);
    printf("Test send_message passed.\n");
}

void test_receive_message() {
    int mq_id = create_message_queue();
    Message msg;
    strcpy(msg.text, "Hello, World!");
    msg.type = 1;
    send_message(mq_id, &msg);

    Message received_msg;
    int result = receive_message(mq_id, &received_msg, 1);
    assert(result != -1);
    assert(strcmp(received_msg.text, "Hello, World!") == 0);
    printf("Test receive_message passed.\n");
}

int main() {
    test_create_message_queue();
    test_send_message();
    test_receive_message();
    printf("All tests passed.\n");
    return 0;
}