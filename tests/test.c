#include "stdio.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_addition(void) {
  printf("This work right");
  int sum = 1 + 1;
  // TEST_FAIL_MESSAGE("so we are unified");
  TEST_ASSERT_EQUAL_INT(sum, 2);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_addition);
  return UNITY_END();
}
