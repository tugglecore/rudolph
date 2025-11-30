#include <stdlib.h>

int rudolph(unsigned int argc, char *argv[]);

int main(int argc, char *argv[]) {
  unsigned int argument_count = argc - 1;

  char **arguments = argument_count == 0 ? NULL : argv + 1;

  rudolph(argument_count, arguments);
}
