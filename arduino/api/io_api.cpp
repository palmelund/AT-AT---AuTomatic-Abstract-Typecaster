#include "io_api.h"

/*
 * Message format:
 *  0xFF
 *  <Size of message in bytes>
 *  <Message type>
 *  <Message data>
 */

/************************
 * Non public functions *
 ************************/
inline uint8_t read_byte()
{
    int read = Serial.read();
    ASSERT(read != -1);

    return (uint8_t)read;
}

/********************
 * Public functions *
 ********************/
void io_send_message(Message* sending_message)
{
    Serial.write(BEGIN_MESSAGE);

    switch (sending_message->type)
    {
    case MESSAGE_TYPE_COMMAND:
        Serial.write(MESSAGE_SIZE_COMMAND);
        Serial.write(sending_message->type);
        Serial.write(sending_message->command.type);
        break;

    default:
        ASSERT(false);
        break;
    }
}

void io_await_message(Message* received_message)
{
    // Wait until data has been received by the Arduino
    while (Serial.available() < 1)
        ;

    uint8_t begin_message = read_byte();
    ASSERT(begin_message == BEGIN_MESSAGE);

    while (Serial.available() < 1)
        ;

    uint8_t message_size = read_byte();
    ASSERT(message_size > 0);

    while (Serial.available() < message_size)
        ;

    received_message->type = read_byte();

    switch (received_message->type)
    {
    case MESSAGE_TYPE_COMMAND:
        ASSERT(message_size == MESSAGE_SIZE_COMMAND);

        received_message->command.type = read_byte();
        break;

    case MESSAGE_TYPE_OBJECT:
        ASSERT(message_size == MESSAGE_SIZE_OBJECT);

        received_message->object.type = read_byte();
        received_message->object.color = read_byte();
        break;

    default:
        ASSERT(false);
        break;
    }
}