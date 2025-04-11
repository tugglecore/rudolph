#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int size;
    char contents[];
} Cell;

typedef struct {
    int size;
    Cell cells[];
} Cells;

int main(void) {
    int BUFFER_SIZE = 1024 * 1024;
    // char filename[] = "users_game_status.csv";
    char filename[] = "sample.csv";

    FILE *fp = fopen(filename, "r");

    if (feof(fp) || ferror(fp)) {
        printf("An error occurred");
        exit(1);
    }

    char delimiters[255] = { [10] = 1, [13] = 1, [44] = 1 };

    while (!feof(fp)) {
        char *buffer = malloc(BUFFER_SIZE);
        int bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);

        char *right_fit_buffer = realloc(buffer, bytes_read);

        if (right_fit_buffer == NULL) {
            printf("Failed to allocate meory");
            exit(1);
        }

        buffer = right_fit_buffer;

        // printf("%s\n", buffer);

        int cursor = 0;
        int count = 0;
        while (cursor < bytes_read && count < 10) {
            int seeker = cursor;
            char token = buffer[seeker];

            int token_is_not_a_delimiter = delimiters[token];
            int seeker_did_not_exceed_bytes_read = seeker < bytes_read;

            while(token_is_not_a_delimiter || seeker_did_not_exceed_bytes_read) {
                printf("Character is %c\n", token);
                printf("Character under test %d\n", token_is_not_a_delimiter);
                seeker++;
                token_is_not_a_delimiter = buffer[seeker];
                seeker_did_not_exceed_bytes_read = seeker < bytes_read;
            }

            cursor = seeker;
            count++;
            printf("On count %d, Cursor is %d and Seeker is %d\n", count, cursor, seeker);
        }

    }

    return 0;
}
