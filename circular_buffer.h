#pragma once

#include <stdlib.h>

#define CIRCULAR_BUFFER_SIZE 80

struct CircularBuffer {
    float values[CIRCULAR_BUFFER_SIZE];
    size_t index, size;
};

void circularBufferInit(struct CircularBuffer *buffer);
void circularBufferAdd(struct CircularBuffer *buffer, float value);
float circularBufferMax(struct CircularBuffer *buffer);
float circularBufferMean(struct CircularBuffer *buffer);
