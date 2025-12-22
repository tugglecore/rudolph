#if __unix__
#define _GNU_SOURCE
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "criterion/criterion.h"     // IWYU pragma: keep
#include "criterion/parameterized.h" // IWYU pragma: keep

char*
join(const char* s1, const char* s2)
{
  char* result = cr_malloc(strlen(s1) + strlen(s2) + 1);

  if (result == NULL) {
    // exit(1);
    return NULL;
  }

  strcpy(result, s1);
  strcat(result, s2);

  return result;
}

char*
build_host_directory(const char directory_stem[])
{
  // (void)directory_stem;
  bool use_std_tmp_dir = true;

  char* directory_stem_template =
    use_std_tmp_dir ? join(join("/rudolph-", directory_stem), "-tests-XXXXXX")
                    : join("/", "/te");
  // (void)directory_stem_template;

#if __unix__
  char* host_location_template = use_std_tmp_dir
                                   ? join("/tmp", directory_stem_template)
                                   : join("./", "tests/artifacts-XXXXXX");

  char* unique_host_directory = mkdtemp(host_location_template);
#endif

  return unique_host_directory;
}

char*
build_test_directory(char* host_directory, char* test_directory)
{
#if __unix__

  char* test_directory_template =
    join(host_directory, join("/", join(test_directory, "-XXXXXX")));

  char* unique_test_directory = mkdtemp(test_directory_template);

#endif

  return unique_test_directory;
}

void
remove_directory(char* directory_name)
{
  (void)system(join("rm -rf ", directory_name)); // NOLINT
}

// NOLINTNEXTLINE
char*
create_file(char* directory, char* filename, char* contents)
{
  char* full_name = join(directory, filename);
  FILE* fp = fopen(full_name, "w+");

  if (fp == NULL) {
    return NULL;
  }

  if (contents == NULL) {
    (void)fclose(fp);
    return full_name;
  }

  size_t size = strlen(contents);

  (void)fwrite(contents, sizeof contents[1], size, fp);
  (void)fclose(fp);

  return full_name;
}

void
redirect(const char* to)
{
  if (freopen(to, "w", stdout) == NULL) {
    exit(1); // NOLINT
  }
}
