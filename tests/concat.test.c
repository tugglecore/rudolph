#include "commands.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void cannot_handle_unknown_commands(void) {}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(cannot_handle_unknown_commands);
  return UNITY_END();
}
