#ifndef _MAIN_
#define _MAIN_

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

#endif // _MAIN_