#ifndef _IO_API_H_
#define _IO_API_H_

#include "Arduino.h"

#define IO_COMMAND_START "command:"
#define IO_COMMAND_END "end"

void io_take_image();
int io_request_result();

#endif // _IO_API_H