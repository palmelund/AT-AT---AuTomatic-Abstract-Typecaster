#ifndef _IO_API_H_
#define _IO_API_H_

#include <stdint.h>
#include "defines.h"
#include "Arduino.h"

#define MAX_MESSAGE_SIZE 3

#define BEGIN_MESSAGE 0x62

// Types of messages that can be received
#define MESSAGE_TYPE_COMMAND 0x00
#define MESSAGE_TYPE_OBJECT 0x01

// Message sizes
#define MESSAGE_SIZE_COMMAND 0x02
#define MESSAGE_SIZE_OBJECT 0x03

// Command types
#define MESSAGE_COMMAND_TAKE_PICURE 0x00
#define MESSAGE_COMMAND_START 0x01
#define MESSAGE_COMMAND_STOP 0x02

// Shapes
#define MESSAGE_SHAPE_BALL 0x00
#define MESSAGE_SHAPE_NOTBALL 0x01

struct Message
{
    uint8_t type;

    union 
    {
        uint8_t data[MAX_MESSAGE_SIZE];

        struct
        {
            uint8_t type;
        } command;

        struct 
        {
            uint8_t type;
            uint8_t color;
        } object;
    };
};

void io_send_message(Message* sending_message);
void io_await_message(Message* received_message);

#endif // _IO_API_H