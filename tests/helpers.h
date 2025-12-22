char*
join(char* s1, char* s2);

char*
build_host_directory(char directory_stem[]);
char*
build_test_directory(char* host_directory, char* test_directory);
char*
create_file(char* directory, char* file, char* contents);
void
remove_directory(char* directory_name);
void
redirect(char* to);
