#include "ring_buffer.h"
#include "unity.h"

#define BUFFER_CAPACITY 8

static RingBuffer rb;
static RingBuffer rb_reject;
static RingBuffer rb_overwrite;
static RingBuffer rb_bulk;
static uint8_t buf[BUFFER_CAPACITY];
static uint8_t buf_reject[BUFFER_CAPACITY];
static uint8_t buf_overwrite[BUFFER_CAPACITY];
static uint8_t buf_bulk[BUFFER_CAPACITY];
static uint8_t src[BUFFER_CAPACITY] = {1, 2, 3, 4, 5, 6, 7, 8};
static uint8_t src_read[BUFFER_CAPACITY];

void setUp(void) { rb_init(&rb, buf, BUFFER_CAPACITY, RB_MODE_REJECT); }

void tearDown(void) { rb_clear(&rb); }

void test_init_state(void) {
  TEST_ASSERT_TRUE(rb_is_empty(&rb));
  TEST_ASSERT_EQUAL_UINT32(0, rb_size(&rb));
}

void test_rb_push(void) {
  TEST_ASSERT_TRUE(rb_push(&rb, 1));
  TEST_ASSERT_EQUAL_UINT32(1, rb_size(&rb));
  TEST_ASSERT_FALSE(rb_is_empty(&rb));
}

void test_rb_push_reject_when_full(void) {
  rb_init(&rb_reject, buf_reject, BUFFER_CAPACITY, RB_MODE_REJECT);
  for (int i = 0; i < BUFFER_CAPACITY; i++) {
    rb_push(&rb_reject, 1);
  }
  TEST_ASSERT_FALSE(rb_push(&rb_reject, 2));
}

void test_rb_push_overwrite_when_full(void) {
  rb_init(&rb_overwrite, buf_overwrite, BUFFER_CAPACITY, RB_MODE_OVERWRITE);
  for (int i = 0; i < BUFFER_CAPACITY; i++) {
    rb_push(&rb_overwrite, 1);
  }
  TEST_ASSERT_TRUE(rb_push(&rb_overwrite, 2));
  uint8_t val;
  rb_pop(&rb_overwrite, &val);
  TEST_ASSERT_EQUAL_UINT8(1, val);
  for (int i = 0; i < BUFFER_CAPACITY - 2; i++) {
    rb_pop(&rb_overwrite, &val);
  }
  rb_pop(&rb_overwrite, &val);
  TEST_ASSERT_EQUAL_UINT8(2, val);
}

void test_rb_peek_test(void) {
  rb_init(&rb, buf, BUFFER_CAPACITY, RB_MODE_REJECT);
  rb_push(&rb, 1);
  uint8_t val;
  rb_peek(&rb, &val);
  TEST_ASSERT_EQUAL_UINT8(1, val);
  TEST_ASSERT_EQUAL_UINT8(1, rb_size(&rb));
}

void test_rb_bulk_write_read(void) {
  rb_init(&rb_bulk, buf_bulk, BUFFER_CAPACITY, RB_MODE_REJECT);
  rb_write(&rb_bulk, src, BUFFER_CAPACITY);
  rb_read(&rb_bulk, src_read, BUFFER_CAPACITY);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(src, src_read, BUFFER_CAPACITY);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_init_state);
  RUN_TEST(test_rb_push);
  RUN_TEST(test_rb_push_reject_when_full);
  RUN_TEST(test_rb_push_overwrite_when_full);
  RUN_TEST(test_rb_peek_test);
  RUN_TEST(test_rb_bulk_write_read);
  return UNITY_END();
}