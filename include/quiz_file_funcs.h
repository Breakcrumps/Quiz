#include <stdio.h>

struct quiz_file;

void free_quiz_file(struct quiz_file quiz_file);
void read_quiz_file(struct quiz_file *quiz_file, FILE *fp);
void write_quiz_file(struct quiz_file quiz_file, FILE *fp);
void print_participant(struct quiz_file quiz_file, int idx);
void print_participants(struct quiz_file quiz_file);
void print_quiz_file(struct quiz_file quiz_file, const char filename[]);
