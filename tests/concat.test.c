// #include "commands.h"
// #include "helpers.h"
// #include "unity.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
//
// char *artifacts_directory = NULL;
//
// void setUp(void) { artifacts_directory = build_host_directory("concat"); }
//
// void tearDown(void) { remove_directory(artifacts_directory); }
//
// void combine_two_csvs_by_columns(void) {
//   char *test_directory =
//       build_test_directory(artifacts_directory,
//       "combine_two_csvs_by_columns");
//
//   create_file(test_directory, "input1.csv", "a,b,c\n1,2,3");
//
//   create_file(test_directory, "input1.csv", "foo,bar\n1800,1900");
//
//   TEST_FAIL();
// }
//
// cppcheck-suppress-file [returnImplicitInt, unknownMacro]
#include <stdio.h>

#include "commands.h"
#include "criterion/criterion.h"     // IWYU pragma: keep
#include "criterion/parameterized.h" // IWYU pragma: keep
#include "helpers.h"

typedef struct
{
  char* location;
  char* input1;
  char* input2;
  char* expected;
  char* actual;
} Workspace;

void
teardown(struct criterion_test_params* ctp)
{
  char* w = ((Workspace*)ctp->params)->location;
  cr_free(w);
}

ParameterizedTestParameters(token_delimited_files, simple)
{
  char* host_directory = build_host_directory("concat_by_column");

  char* location = cr_malloc(strlen(host_directory));
  strcpy(location, host_directory);
  cr_free(host_directory);

  static Workspace workspaces[1];
  char* actual = create_file(location, "/actual.csv", NULL);
  char* input1 = create_file(location, "/input1.csv", "a,b,c\n1,2,3");
  char* input2 = create_file(location, "/input2.csv", "d,e,f\n4,5,6");
  char* expected =
    create_file(location, "/expected.csv", "a,b,c,d,e,f\n1,2,3,4,5,6");
  Workspace wp = { .location = location,
                   .input1 = input1,
                   .input2 = input2,
                   .expected = expected,
                   .actual = actual };
  workspaces[0] = wp;

  // redirect(actual);
  return cr_make_param_array(
    Workspace, workspaces, sizeof(workspaces) / sizeof(Workspace), teardown);
}

ParameterizedTest(Workspace* workspace, token_delimited_files, simple)
{
  char* arguments[] = { "columns", workspace->input1, workspace->input2 };

  int result = concat(3, arguments);

  cr_assert(result);
}
