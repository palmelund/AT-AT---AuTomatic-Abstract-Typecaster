#ifndef _PROTOTYPE_3_H_
#define _PROTOTYPE_3_H_

enum Ball_Color 
{
  GREEN, 
  YELLOW, 
  RED, 
  BLUE, 
  EMPTY
};

struct Segment_Queue 
{
  Ball_Color data[QUEUE_SIZE] = { EMPTY };
  uint8_t index = 0;
};

#endif // _PROTOTYPE_3_H_