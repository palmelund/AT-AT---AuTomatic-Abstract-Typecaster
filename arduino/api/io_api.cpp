#include "io_api.h"

void io_take_image()
{
    Serial.println(IO_COMMAND_START);
    Serial.println('2');    // TODO: this is what we send. May need to change
    Serial.println(IO_COMMAND_END);
}

int io_request_result()
{
    return Serial.read();
}