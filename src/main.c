#include <stdlib.h>
#include <stdio.h>
#include "commands.h"

#define STATS 210728208916 
#define ECHO 6385181892 

const unsigned long hash(const char *str) {
    unsigned long hash = 5381;  
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("How many you got: %d\n", argc);
        printf("[ERROR] Not enough arguments\n");
        return EXIT_FAILURE;
    }

    int argument_count = argc - 2;
    char ** arguments = &argv[2];

    char *command = argv[1];
    switch (hash(command)) {
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
    }

  return 0;
}
