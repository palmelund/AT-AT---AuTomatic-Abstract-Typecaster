#ifndef _IO_API_H_
#define _IO_API_H_

#include <stdint.h>
#include "defines.h"
#include "Arduino.h"

#define MAX_MESSAGE_SIZE 3

#define BEGIN_MESSAGE 0x62

// Types of messages that can be received
#define IN_MESSAGE_COMMAND 0
#define IN_MESSAGE_OBJECT 1
#define IN_MESSAGE_COLOR 2
#define IN_MESSAGE_DISTANCE 3

// Input message sizes
#define IN_SIZE_COMMAND 2
#define IN_SIZE_OBJECT 2
#define IN_SIZE_COLOR 8
#define IN_SIZE_DISTANCE 3

// Input command types
#define IN_COMMAND_CALIBRATE_RED 0
#define IN_COMMAND_CALIBRATE_GREEN 1
#define IN_COMMAND_CALIBRATE_BLUE 2
#define IN_COMMAND_CALIBRATE_YELLOW 3
#define IN_COMMAND_CALIBRATE_DISTANCE 4
#define IN_COMMAND_START 5
#define IN_COMMAND_STOP 6

struct In_Message
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
        } object;

        struct
        {
            uint8_t type;
            uint16_t red_value;
            uint16_t green_value;
            uint16_t blue_value;
        } color;

        struct 
        {
            uint16_t value;
        } distance;
    };
};

// Types of messages that can be sent
#define OUT_MESSAGE_COMMAND 0
#define OUT_MESSAGE_REQUEST 1
#define OUT_MESSAGE_COLOR 2
#define OUT_MESSAGE_DISTANCE 3

// Output message sizes
#define OUT_SIZE_COMMAND 2
#define OUT_SIZE_REQUEST 2
#define OUT_SIZE_COLOR 8
#define OUT_SIZE_DISTANCE 3

// Output command types
#define OUT_COMMAND_TAKE_PICURE 0

// Output request types
#define OUT_REQUEST_OBJECT_INFO 0

struct Out_Message 
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
        } request;

        struct
        {
            uint8_t type;
            uint16_t red_value;
            uint16_t green_value;
            uint16_t blue_value;
        } color;

        struct
        {
            uint16_t value;
        } distance;
    };
};

void io_send_message(Out_Message* sending_message);
void io_await_message(In_Message* received_message);

#endif // _IO_API_H