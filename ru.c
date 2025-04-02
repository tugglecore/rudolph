#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *contents;
} Cell;

int main(void) {
    int BUFFER_SIZE = 1024 * 1024;
    char filename[] = "users_game_status.csv";

    FILE *fp = fopen(filename, "r");

    if (feof(fp) || ferror(fp)) {
        printf("An error occurred");
        exit(1);
    }

    while (!feof(fp)) {
        char *buffer = malloc(BUFFER_SIZE);
        int bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);

        char *right_fit_buffer = realloc(buffer, bytes_read);

        if (right_fit_buffer == NULL) {
            printf("Failed to allocate meory");
            exit(1);
        }

        buffer = right_fit_buffer;

        printf("%s\n", buffer);
    }

    return 0;
}
