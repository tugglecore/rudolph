#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  char file_name[] = "sample.csv";
  char **cells = NULL;
  int buffer_size = 1 * 1024 * 1024;
  char *buffer = malloc(buffer_size);

  printf("What is in here: %d\n", buffer[7]);

  FILE *fp = fopen("sample.csv", "r");

  if (fp == NULL) {
      perror("Error opening file");
      return 1;
  }

  int amount_of_bytes_read = fread(buffer, 1, buffer_size,  fp);

  if (ferror(fp)) {
      perror("Error reading file");
      fclose(fp);
      return 1;
  }

  printf("amount of bytes read %d\n", amount_of_bytes_read);
  // buffer[amount_of_bytes_read] = '\0';

  printf("The size of the array is: %lu\n", sizeof(buffer));
  printf("The amount of bytes in array is: %lu\n", strlen(buffer));
  printf("%s\n", buffer);
  printf("Here is an element: %d", buffer[amount_of_bytes_read+1]);

  char *right_fit_buffer = realloc(buffer, strlen(buffer) + 1);

  if (right_fit_buffer != NULL)
    buffer = right_fit_buffer;

  printf("The size of the array is: %lu\n", sizeof(buffer));
  printf("The amount of bytes in array is: %lu\n", strlen(buffer));
  printf("%s\n", buffer);

  return 0;
}
