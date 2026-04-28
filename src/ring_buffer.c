#include "ring_buffer.h"
#include <string.h>

/* ── Status ── */
bool rb_is_empty(const RingBuffer *rb) { return rb->head == rb->tail; }

bool rb_is_full(const RingBuffer *rb) {
  return rb->tail == rb->head + rb->capacity;
}

uint32_t rb_size(const RingBuffer *rb) { return rb->tail - rb->head; }

uint32_t rb_free(const RingBuffer *rb) { return rb->capacity - rb_size(rb); }

/* ── Initialization ── */
static inline bool is_power_of_two(uint32_t n) {
  return (n > 0) && ((n & (n - 1)) == 0);
}

bool rb_init(RingBuffer *rb, uint8_t *buf, uint32_t capacity,
             RingBufferMode mode) {
  if (rb == NULL || buf == NULL) {
    return false;
  }
  if (!is_power_of_two(capacity)) {
    return false;
  }
  if (mode != RB_MODE_REJECT && mode != RB_MODE_OVERWRITE) {
    return false;
  }

  rb->buf = buf;
  rb->head = 0;
  rb->tail = 0;
  rb->capacity = capacity;
  rb->mode = mode;

  return true;
}

/* ── Utility ── */
void rb_clear(RingBuffer *rb) {
  rb->head = 0;
  rb->tail = 0;
}

/* ── Single Byte Operations ── */
bool rb_push(RingBuffer *rb, uint8_t data) {
  if (rb_is_full(rb)) {
    if (rb->mode == RB_MODE_REJECT) {
      return false;
    }
    if (rb->mode == RB_MODE_OVERWRITE) {
      rb->head++;
    }
  }

  rb->buf[rb->tail & (rb->capacity - 1)] = data;
  rb->tail++;

  return true;
}

bool rb_pop(RingBuffer *rb, uint8_t *data) {
  if (rb_is_empty(rb)) {
    return false;
  }
  *data = rb->buf[rb->head & (rb->capacity - 1)];
  rb->head++;
  return true;
}