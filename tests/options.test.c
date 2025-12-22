// cppcheck-suppress-file returnImplicitInt
#include "criterion/criterion.h" // IWYU pragma: keep

Test(misc, failing)
{
  cr_assert(0);
}

Test(misc, passing)
{
  cr_assert(1);
}
// #include "unity.h"
//
// void setUp(void) {}
//
// void tearDown(void) {}
//
// void rudolph_fails_without_arguments(void) {
//   int exit_status = rudolph(0, NULL);
//   TEST_ASSERT_TRUE(exit_status);
// }
//
// void rudolph_can_help(void) {
//   char *arguments[] = {"help"};
//   int exit_status = rudolph(1, arguments);
//   TEST_ASSERT_FALSE(exit_status);
//
//   arguments[0] = "--help";
//   exit_status = rudolph(1, arguments);
//   TEST_ASSERT_FALSE(exit_status);
// }
//
// void say_version(void) {
//   char *arguments[] = {"--version"};
//   int exit_status = rudolph(1, arguments);
//   TEST_ASSERT_FALSE(exit_status);
// }
//
// void cannot_handle_unknown_commands(void) {
//   char *arguments[] = {"unknown_and_invalid"};
//   int exit_status = rudolph(1, arguments);
//   TEST_ASSERT_TRUE(exit_status);
// }
//
// int main(void) {
//     printf("Hold your fire");
//     return 0;
//   // UNITY_BEGIN();
//   // RUN_TEST(say_version);
//   // RUN_TEST(rudolph_can_help);
//   // RUN_TEST(cannot_handle_unknown_commands);
//   // RUN_TEST(rudolph_fails_without_arguments);
//   // return UNITY_END();
// }
