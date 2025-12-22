#include "commands.h"
#include "unity.h"

void
setUp(void)
{
}

void
tearDown(void)
{
}

void
read_comma_delimited_csv(void)
{
  Csv* csv = reader("./comma_delimited.csv");
  if (csv == NULL) {
    TEST_FAIL_MESSAGE("Failed to create CSV");
  }

  printf("What is the delimiter: %c\n", csv->delimiter);
  TEST_ASSERT_EQUAL_CHAR_MESSAGE(
    ',', csv->delimiter, "Expecting delimiter to be a comma");
}

void
read_semicolon_delimited_csv(void)
{
  Csv* csv = reader("./semicolon_delimited.ssv");
  if (csv == NULL) {
    TEST_FAIL_MESSAGE("Failed to create CSV");
  }

  printf("What is the delimiter: %c\n", csv->delimiter);
  TEST_ASSERT_EQUAL_CHAR_MESSAGE(
    ';', csv->delimiter, "Expecting delimiter to be a comma");
}

int
main(void)
{
  UNITY_BEGIN();
  RUN_TEST(read_comma_delimited_csv);
  RUN_TEST(read_semicolon_delimited_csv);
  return UNITY_END();
}
