#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <threads.h>

typedef struct {
  int length;
  long long int start;
  long long int end;
  char contents[];
} Cell;

typedef struct {
  int size;
  char name[];
} Heading;

typedef struct {
  int amount_of_headings;
  Heading *headings[];
} Header;

typedef struct {
  Header *header;
  int row_size;
  int amount_of_cells;
  char separator;
  Cell *cells[];
} Csv;

typedef struct {
  thrd_t thread;
  char *file_contents;
  int start;
  int end;
  char ** output;
  int output_size;
} Partition;

int process_csv(void *arg);

int main(void) {
  int CELL_BLOCK = 1024 * 1024;
  int allocated_cells = CELL_BLOCK;

  int BUFFER_SIZE = 1024 * 1024;

  char filename[] = "sample.csv";

  FILE *fp = fopen(filename, "r");

  if (feof(fp) || ferror(fp)) {
    printf("An error occurred");
    exit(1);
  }

  int fd = fileno(fp);

  struct stat *file_attributes = malloc(sizeof(struct stat));

  if (fstat(fd, file_attributes)) {
    perror("fstat");
    return 1;
  }
  int file_size = file_attributes->st_size;

  char *mapping;

  mapping = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0);

  if (mapping == MAP_FAILED) {
    perror("mmap");
    return 1;
  }

  // TODO: realloc Headers with > 100 headings
  Header *header = malloc(sizeof(Header) + 100 * sizeof(Heading *));
  header->amount_of_headings = 0;

  int line_cursor = 0;

  while (line_cursor < file_size) {
    // TODO: Handle carriage returns
    // TODO: Handle empty header rows

    int start_of_heading = line_cursor;
    int heading_cursor = line_cursor;

    do {
      if (mapping[heading_cursor] == ',' || mapping[heading_cursor] == '\n')
        break;
    } while (heading_cursor++, heading_cursor < file_size);

    // TODO: Handle empty headings
    int heading_size = heading_cursor - start_of_heading;

    Heading *heading = malloc(sizeof(Heading) + sizeof(char) * heading_size);

    heading->size = heading_size;

    memcpy(&heading->name, &mapping[start_of_heading], heading_size);
    header->headings[header->amount_of_headings] = heading;
    header->amount_of_headings++;

    line_cursor = heading_cursor + 1;
    if (mapping[heading_cursor] == '\n') {
      break;
    }
  }

  int start_of_data = line_cursor;
  int remaining_data_in_file = file_size - start_of_data;

  int amount_of_cores = get_nprocs();

  int size_of_partitions = remaining_data_in_file / amount_of_cores;

  Partition * partitions[amount_of_cores];
  int amount_of_threads_created = 0;
  int partition_start_cursor = line_cursor;


  while (partition_start_cursor < file_size) {
    Partition *partition = malloc(sizeof(Partition));
    partition->file_contents = mapping;
    partition->start = partition_start_cursor;
    partition->output = malloc(sizeof(char *));

    if (amount_of_threads_created == amount_of_cores - 1) {
        if (mapping[file_size -1] == '\n') {
            partition->end = file_size - 2;
        } else {
          partition->end = file_size - 1;
        }
    } else {

      int estimated_partition_endpoint =
          partition_start_cursor + size_of_partitions;
      int partition_end_cursor = estimated_partition_endpoint;

      // TODO: Last partition no matter what; so create thread!
      if (partition_end_cursor >= file_size) {
        partition_end_cursor = file_size - 1;
      }

      // TODO: Handle empty row
      int alteration = -1;
      bool partition_end_cursor_went_forward = false;

      while (true) {
        if (mapping[partition_end_cursor] == '\n')
          break;

        // end_cursor is at last position in file
        if (partition_end_cursor + 1 == file_size)
          break;

        partition_end_cursor += alteration;

        if (partition_end_cursor == partition_start_cursor) {
          partition_end_cursor = estimated_partition_endpoint;
          alteration = 1;
          partition_end_cursor_went_forward = true;
        }
      }

      bool end_cursor_at_last_position = partition_end_cursor == file_size - 1;
      if (end_cursor_at_last_position) {
          if (partition_end_cursor == '\n') {
              partition->end = partition_end_cursor - 1;
          } else {
        partition->end = partition_end_cursor;
          }
      } else {
        partition->end = partition_end_cursor - 1;
      }

      partition_start_cursor = partition_end_cursor + 1;

      // int size_of_partitions = remaining_data_in_file / amount_of_cores;
      if (partition_end_cursor_went_forward && !end_cursor_at_last_position) {
      }
    }

    thrd_t thread;

    thrd_create(&thread, process_csv, partition);
    partition->thread = thread;
    partitions[amount_of_threads_created] = partition;
    amount_of_threads_created++;

    if (amount_of_threads_created == amount_of_cores) 
        break;
  }

  struct stat *stdout_attributes = malloc(sizeof(struct stat));

  int a = fileno(stdout);
  fstat(a, file_attributes);

  for (int i = 0; i < amount_of_threads_created; i++) {
      int res = 0;
      Partition *partition = partitions[i];
      thrd_join(partition->thread, &res);
      if (res)
          return -1;

      char * output = *partition->output;
      int output_size = partition->output_size;

      printf("%.*s", output_size, output);
  }

  return 0;
}

int process_csv(void *arg) {
  Partition *partition = (Partition *)arg;
 
  // (end - start) + 1 for inclusive range + 1 for trailing newline
  char *output = malloc((partition->end - partition->start) + 1 + 1);

  int output_cursor = 0;
  int input_cursor = partition->start;

  // printf("What was the last character: %d\n", partition->file_contents[partition->end]);

  while (input_cursor <= partition->end) {
      output[output_cursor] = partition->file_contents[input_cursor];
      input_cursor++;
      output_cursor++;
  }

  output[output_cursor] = '\n';
  *partition->output = output;
  partition->output_size = output_cursor + 1;

  return 0;
}
