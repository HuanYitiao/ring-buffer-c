#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Mode ── */

/**
 * @brief Behavior policy when the buffer is full
 *
 * RB_MODE_REJECT    - Push returns false, new data is discarded.
 *                     Use when data loss is unacceptable (e.g. command queues).
 *
 * RB_MODE_OVERWRITE - Oldest data is silently overwritten.
 *                     Use when latest data matters most (e.g. sensor streams,
 * logs).
 */
typedef enum {
  RB_MODE_REJECT,
  RB_MODE_OVERWRITE,
} RingBufferMode;

/* ── Data Structure ── */

/**
 * @brief Ring buffer control structure
 *
 * Memory layout is fixed at init time and never moves.
 * The same physical block is reused indefinitely via head/tail indices.
 * This makes it safe for DMA and zero-fragmentation embedded use.
 *
 * Capacity must be a power of 2 to enable fast bitwise modulo:
 *   index & (capacity - 1)  instead of  index % capacity
 */
typedef struct {
  uint8_t *buf;        /* Pointer to externally provided memory block  */
  uint32_t head;       /* Read index  (consumer moves this forward)    */
  uint32_t tail;       /* Write index (producer moves this forward)    */
  uint32_t capacity;   /* Size of buf in bytes, must be power of 2     */
  RingBufferMode mode; /* Full-buffer behavior: reject or overwrite     */
} RingBuffer;

/* ── Initialization ── */

/**
 * @brief Initialize a ring buffer instance
 * @param rb       Pointer to RingBuffer struct
 * @param buf      Externally allocated memory block (must remain valid)
 * @param capacity Size of buf in bytes (must be a power of 2)
 * @param mode     Behavior when buffer is full
 * @return true on success, false if arguments are invalid
 */
bool rb_init(RingBuffer *rb, uint8_t *buf, uint32_t capacity,
             RingBufferMode mode);

/* ── Single Byte Operations ── */

/**
 * @brief Push one byte into the buffer
 * @param rb   Pointer to RingBuffer struct
 * @param data Byte to write
 * @return true on success
 *         false if full and mode is RB_MODE_REJECT
 *         always true if mode is RB_MODE_OVERWRITE
 */
bool rb_push(RingBuffer *rb, uint8_t data);

/**
 * @brief Pop one byte from the buffer (removes it)
 * @param rb   Pointer to RingBuffer struct
 * @param data Output pointer to store the read byte
 * @return true on success, false if buffer is empty
 */
bool rb_pop(RingBuffer *rb, uint8_t *data);

/**
 * @brief Peek at the next byte without removing it
 * @param rb   Pointer to RingBuffer struct
 * @param data Output pointer to store the peeked byte
 * @return true on success, false if buffer is empty
 */
bool rb_peek(const RingBuffer *rb, uint8_t *data);

/* ── Bulk Operations ── */

/**
 * @brief Write multiple bytes into the buffer
 * @param rb  Pointer to RingBuffer struct
 * @param src Source data pointer
 * @param len Number of bytes to write
 * @return Number of bytes actually written
 */
uint32_t rb_write(RingBuffer *rb, const uint8_t *src, uint32_t len);

/**
 * @brief Read multiple bytes from the buffer
 * @param rb  Pointer to RingBuffer struct
 * @param dst Destination buffer pointer
 * @param len Number of bytes to read
 * @return Number of bytes actually read
 */
uint32_t rb_read(RingBuffer *rb, uint8_t *dst, uint32_t len);

/* ── Status ── */

/**
 * @brief Get number of bytes available to read
 * @param rb Pointer to RingBuffer struct
 * @return Bytes stored in buffer
 */
uint32_t rb_size(const RingBuffer *rb);

/**
 * @brief Get number of bytes available to write
 * @param rb Pointer to RingBuffer struct
 * @return Free bytes remaining
 */
uint32_t rb_free(const RingBuffer *rb);

/**
 * @brief Check if buffer is empty
 */
bool rb_is_empty(const RingBuffer *rb);

/**
 * @brief Check if buffer is full
 */
bool rb_is_full(const RingBuffer *rb);

/* ── Utility ── */

/**
 * @brief Reset buffer to empty state (does not zero memory)
 */
void rb_clear(RingBuffer *rb);

#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H */