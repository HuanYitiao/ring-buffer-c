#include "ring_buffer.h"
#include "unity.h"

#define BUFFER_CAPACITY 8

static RingBuffer rb;
static RingBuffer rb_reject;
static RingBuffer rb_overwrite;
static uint8_t buf[BUFFER_CAPACITY];
static uint8_t buf_reject[BUFFER_CAPACITY];

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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_init_state);
  RUN_TEST(test_rb_push);
  RUN_TEST(test_rb_push_reject_when_full);
  return UNITY_END();
}