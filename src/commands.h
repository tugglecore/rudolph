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
  long int start;
  long int end;
  void *output;
} Partition;

typedef struct {
  char *file_contents;
  Header *header;
  char delimiter;
  int amount_of_partitions;
  Partition *partitions[];
} Csv;

Csv *reader(char *filename);

int slice(int argument_count, char *arguments[]);
int stats(int argument_count, char *arguments[]);
int echo(int argument_count, char *arguments[]);
