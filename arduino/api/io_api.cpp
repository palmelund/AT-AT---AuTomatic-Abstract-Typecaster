#include "io_api.h"

void io_send(uint8_t msg)
{
    Serial.print('b'); 
    Serial.print(msg); 
    Serial.print('e');
}

uint8_t io_recieve()
{
    
}