#include "circular_buffer.h"

#include <math.h>
#include <stdlib.h>

void circularBufferInit(struct CircularBuffer* buffer) {
    buffer->size = 0;
    buffer->index = 0;
}

void circularBufferAdd(struct CircularBuffer* buffer, float value) {
    if (buffer->size < CIRCULAR_BUFFER_SIZE) {
        buffer->size++;
    }

    buffer->values[buffer->index] = value;
    buffer->index = (buffer->index + 1) % CIRCULAR_BUFFER_SIZE;
}

float circularBufferMax(struct CircularBuffer* buffer) {
    float result = NAN;
    size_t index = (buffer->index + (CIRCULAR_BUFFER_SIZE - buffer->size)) % CIRCULAR_BUFFER_SIZE;

    for (size_t i = 0; i < buffer->size; i++) {
        if (isnan(result) || result < buffer->values[index]) {
            result = buffer->values[index];
        }

        index = (index + 1) % CIRCULAR_BUFFER_SIZE;
    }

    return result;
}

float circularBufferMean(struct CircularBuffer* buffer) {
    float sum = 0;
    for (size_t i = 0; i < buffer->size; i++) {
        sum += buffer->values[i];
    }

    return sum / buffer->size;
}
