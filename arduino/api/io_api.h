#ifndef _IO_API_H_
#define _IO_API_H_

#include <stdint.h>

#include "Arduino.h"

void io_send(uint8_t msg);
uint8_t io_recieve();

#endif // _IO_API_H