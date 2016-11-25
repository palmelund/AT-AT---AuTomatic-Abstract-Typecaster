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
void io_send_message(Out_Message *sending_message)
{
    Serial.write(BEGIN_MESSAGE);

    switch (sending_message->type)
    {
    case OUT_MESSAGE_COLOR:
        Serial.write(OUT_SIZE_COLOR);
        Serial.write(OUT_MESSAGE_COLOR);

        for (int i = 0; i < OUT_SIZE_COLOR - 1; i++)
            Serial.write(sending_message->data[i]);
        break;

    case OUT_MESSAGE_COMMAND:
        Serial.write(OUT_SIZE_COMMAND);
        Serial.write(OUT_MESSAGE_COMMAND);
        Serial.write(sending_message->command.type);
        break;

    case OUT_MESSAGE_DISTANCE:
        Serial.write(OUT_SIZE_DISTANCE);
        Serial.write(OUT_MESSAGE_DISTANCE);

        for (int i = 0; i < OUT_SIZE_DISTANCE - 1; i++)
            Serial.write(sending_message->data[i]);
        break;

    case OUT_MESSAGE_REQUEST:
        Serial.write(OUT_SIZE_REQUEST);
        Serial.write(OUT_MESSAGE_REQUEST);
        Serial.write(sending_message->request.type);
        break;

    default:
        ASSERT(false);
        break;
    }
}

void io_await_message(In_Message *received_message)
{
    // Wait until data has been received by the Arduino
    while (Serial.available() < 2)
    {
    }

    uint8_t begin_message = read_byte();

    uint8_t message_size = read_byte();

    ASSERT(begin_message == BEGIN_MESSAGE);
    ASSERT(message_size > 0);
    while (Serial.available() < message_size)
    {
    }

    received_message->type = read_byte();

    switch (received_message->type)
    {
    case IN_MESSAGE_COLOR:
        ASSERT(message_size == IN_SIZE_COLOR);

        received_message->color.type = read_byte();

        received_message->color.red_value = read_byte();
        received_message->color.red_value <<= 8;
        received_message->color.red_value |= read_byte();

        received_message->color.green_value = read_byte();
        received_message->color.green_value <<= 8;
        received_message->color.green_value |= read_byte();

        received_message->color.blue_value = read_byte();
        received_message->color.blue_value <<= 8;
        received_message->color.blue_value |= read_byte();
        break;

    case IN_MESSAGE_COMMAND:
        ASSERT(message_size == IN_SIZE_COMMAND);

        received_message->command.type = read_byte();
        break;

    case IN_MESSAGE_DISTANCE:
        ASSERT(message_size == IN_SIZE_DISTANCE);

        received_message->distance.value = read_byte();
        received_message->distance.value <<= 8;
        received_message->distance.value |= read_byte();
        break;

    case IN_MESSAGE_OBJECT:
        ASSERT(message_size == IN_SIZE_OBJECT);

        received_message->object.type = read_byte();
        break;

    default:
        ASSERT(false);
        break;
    }
}