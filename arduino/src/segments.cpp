#include "segments.h"

Segment* queue_next(Segment_Queue *segment_queue)
{
    segment_queue->index++;
    if (segment_queue->index >= QUEUE_SIZE)
        segment_queue->index = 0;

    return &segment_queue->data[segment_queue->index];
}

Segment* get_segment(Segment_Queue *segment_queue, uint8_t offset)
{
    return &segment_queue->data[(segment_queue->index + offset) % QUEUE_SIZE];
}