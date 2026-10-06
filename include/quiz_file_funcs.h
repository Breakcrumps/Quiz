struct quiz_file;
struct _iobuf;

void read_quiz_file(struct quiz_file *quiz_file, struct _iobuf *fp);
void print_quiz_file(struct quiz_file quiz_file, const char filename[]);
void print_global_stats(struct quiz_file quiz_file);
void print_per_question_stats(struct quiz_file quiz_file);
void print_per_participant_stats(struct quiz_file quiz_file);
