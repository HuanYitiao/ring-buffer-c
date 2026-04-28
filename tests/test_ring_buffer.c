#include "ring_buffer.h"
#include "unity.h"

static RingBuffer rb;
static uint8_t buf[8];

void setUp(void) { rb_init(&rb, buf, 8, RB_MODE_REJECT); }

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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_init_state);
  RUN_TEST(test_rb_push);
  return UNITY_END();
}