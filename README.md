# ring-buffer-c

---
## What is this

A lightweight ring buffer library written in C, designed for embedded systems and resource-constrained environments.

It operates on a fixed, externally provided memory block with no dynamic allocation, making it safe for bare-metal and RTOS-based projects. Typical use cases include UART receive buffers,sensor data streaming, and producer-consumer pipelines.

Key properties:
- No heap allocation — buffer memory is provided by the caller
- Two full-buffer policies: reject new data or overwrite oldest
- Power-of-2 capacity for fast bitwise indexing
- C99 compatible, no external dependencies

## Features

- Fixed-size circular buffer with no dynamic memory allocation
- Caller-provided memory — works on stack, static, or heap
- Two full-buffer policies: `RB_MODE_REJECT` and `RB_MODE_OVERWRITE`
- Power-of-2 capacity for fast bitwise indexing
- C99 compatible, no external dependencies
- C++ compatible via `extern "C"`
- Not thread-safe by default — external locking required for concurrent access
- Unit tested with Unity — covers initialization, boundary conditions, bulk read/write

## Installation / Integration

### Option 1: Copy files directly (recommended for embedded)

Copy the following files into your project:

- `include/ring_buffer.h`
- `src/ring_buffer.c`

Then include the header in your source:

```c
#include "ring_buffer.h"
```

### Option 2: CMake FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    ring_buffer
    GIT_REPOSITORY https://github.com/HuanYitiao/ring-buffer-c.git
    GIT_TAG v0.1.1
)
FetchContent_MakeAvailable(ring_buffer)
target_link_libraries(your_target ring_buffer)
```

## Examples

### Basic usage (REJECT mode)

```c
#include "ring_buffer.h"

static RingBuffer rb;
static uint8_t buf[8];

int main(void)
{
    rb_init(&rb, buf, 8, RB_MODE_REJECT);

    rb_push(&rb, 0x01);
    rb_push(&rb, 0x02);
    rb_push(&rb, 0x03);

    uint8_t val;
    rb_pop(&rb, &val);  /* val = 0x01 */
    rb_pop(&rb, &val);  /* val = 0x02 */

    return 0;
}
```

### Overwrite mode (sensor streaming)

```c
static RingBuffer rb;
static uint8_t buf[8];

int main(void)
{
    rb_init(&rb, buf, 8, RB_MODE_OVERWRITE);

    /* Fill buffer */
    for (int i = 0; i < 8; i++) {
        rb_push(&rb, i);
    }

    /* Oldest data is overwritten, latest is kept */
    rb_push(&rb, 0xFF);

    uint8_t val;
    rb_pop(&rb, &val);  /* val = 0x01, not 0x00 */

    return 0;
}
```

### Bulk read / write

```c
static RingBuffer rb;
static uint8_t buf[16];

int main(void)
{
    rb_init(&rb, buf, 16, RB_MODE_REJECT);

    uint8_t tx[] = {1, 2, 3, 4, 5};
    rb_write(&rb, tx, sizeof(tx));

    uint8_t rx[5];
    rb_read(&rb, rx, sizeof(rx));
    /* rx now contains {1, 2, 3, 4, 5} */

    return 0;
}
```
## API Reference

### Initialization

| Function | Description |
|----------|-------------|
| `bool rb_init(RingBuffer *rb, uint8_t *buf, uint32_t capacity, RingBufferMode mode)` | Initialize a ring buffer instance |

### Single Byte Operations

| Function | Description |
|----------|-------------|
| `bool rb_push(RingBuffer *rb, uint8_t data)` | Push one byte into the buffer |
| `bool rb_pop(RingBuffer *rb, uint8_t *data)` | Pop one byte from the buffer |
| `bool rb_peek(const RingBuffer *rb, uint8_t *data)` | Peek at next byte without removing it |

### Bulk Operations

| Function | Description |
|----------|-------------|
| `uint32_t rb_write(RingBuffer *rb, const uint8_t *src, uint32_t len)` | Write multiple bytes |
| `uint32_t rb_read(RingBuffer *rb, uint8_t *dst, uint32_t len)` | Read multiple bytes |

### Status

| Function | Description |
|----------|-------------|
| `uint32_t rb_size(const RingBuffer *rb)` | Bytes available to read |
| `uint32_t rb_free(const RingBuffer *rb)` | Bytes available to write |
| `bool rb_is_empty(const RingBuffer *rb)` | Check if buffer is empty |
| `bool rb_is_full(const RingBuffer *rb)` | Check if buffer is full |
| `void rb_clear(RingBuffer *rb)` | Reset buffer to empty state |

### Modes

| Mode | Description |
|------|-------------|
| `RB_MODE_REJECT` | Return false when full, new data discarded |
| `RB_MODE_OVERWRITE` | Overwrite oldest data when full |

## License

MIT License — see [LICENSE](LICENSE) for details.