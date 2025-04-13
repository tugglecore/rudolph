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
        
        printf("Total bytes read: %d: \n", bytes_read);
        buffer = right_fit_buffer;

        int cursor = 0;
        int count = 0;
        while (cursor < bytes_read - 1 && count < 10) {
            int seeker = cursor;
            char token = buffer[seeker];

            int token_is_not_a_delimiter = delimiters[token];
            int seeker_did_not_exceed_bytes_read = seeker < bytes_read - 1;

            while(1) {
                // Do something with the current token/seeker
                printf("At the seeker position of %d, is the character '%c', a delimiter? %d\n", seeker, token, token_is_not_a_delimiter);

                if (seeker == bytes_read - 1 || delimiters[buffer[seeker]])  { break; }

                // move the seeker
                seeker++;
                token = buffer[seeker];
                token_is_not_a_delimiter = delimiters[token];
                seeker_did_not_exceed_bytes_read = seeker < bytes_read - 1;
            }

            cursor = seeker + 1;
            count++;
            printf("On count %d, Cursor is %d and Seeker is %d\n", count, cursor, seeker);
        }

    }

    return 0;
}
