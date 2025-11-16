typedef struct {
  int size;
  char name[];
} Heading;

typedef struct {
  int amount_of_headings;
  Heading *headings[];
} Header;

typedef struct {
  char *file_contents;
  int start;
  int end;
  void * output;
} Partition;

typedef struct {
  Header *header;
  char delimiter;
  int amount_of_partitions;
  Partition * partitions[];
} Csv;

Csv* reader(char * filename);

void stats(int argument_count, char *argv[]);
void echo(int argument_count, char *argv[]);
