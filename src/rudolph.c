#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLICE 210727929557
#define STATS 210728208916
#define ECHO 6385181892

const char *version = "0.1";

void help(const char *command);

unsigned long hash(const char *input) {
  unsigned long hash = 5381;
  int c;

  const unsigned char *string = (unsigned char *)input;

  while ((c = *string++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

int rudolph(int argc, char *argv[]) {
  if (argc == 0) {
    help(NULL);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[0], "--version") == 0) {
    printf("version %s\n", version);
    return EXIT_SUCCESS;
  }

  if (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "help") == 0) {
    if (argc > 1) {
      help(argv[1]);
    } else {
      help(NULL);
    }

    return EXIT_SUCCESS;
  }

  if (argc < 2) {
    printf("[ERROR] Not enough arguments\n");
    return EXIT_FAILURE;
  }

  const char *command = argv[0];
  int argument_count = argc - 1;
  char **arguments = argv + 1;
  switch (hash(command)) {
  case SLICE:
    printf("Running slice...\n");
    slice(argument_count, arguments);
    break;
  case STATS:
    printf("Running count...\n");
    stats(argument_count, arguments);
    break;
  case ECHO:
    printf("Running echo...\n");
    echo(argument_count, arguments);
    break;
  default:
    printf("[ERROR] %s is not a valid command.\n", command);
    return EXIT_FAILURE;
  }

  return 0;
}

const char *help_text = "\
CSV toolkit \
";

void help(const char *command) {
  if (command == NULL) {
    printf("%s\n", help_text);
  }
}
