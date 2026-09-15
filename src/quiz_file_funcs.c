#include "../include/quiz_file_funcs.h"
#include <stdlib.h>
#include <string.h>
#include "../include/str.h"
#include "../include/console.h"
#include "../include/questions.h"

void free_quiz_file(QuizFile quiz_file)
{
  for (int i = 0; i < quiz_file.record_count; i++)
  {
    free(quiz_file.records[i].note);
  }

  free(quiz_file.records);
}

void read_quiz_file(QuizFile *quiz_file, FILE *fp)
{
  fread(&quiz_file->record_count, sizeof(u16), 1, fp);
  fread(&quiz_file->reject_count, sizeof(u16), 1, fp);

  quiz_file->records = calloc(quiz_file->record_count, sizeof(Record));

  if (!quiz_file->records)
  {
    report_alloc_error();
    return;
  }

  for (int i = 0; i < quiz_file->record_count; i++)
  {
    fread(quiz_file->records[i].answers, QUESTION_ARR_SIZE, 1, fp);

    quiz_file->records[i].note = fread_str_dynamic(fp);

    if (!quiz_file->records[i].note)
    {
      report_alloc_error();
      free_quiz_file(*quiz_file);
      return;
    }
    
    if (quiz_file->records[i].note[0] == '\0')
    {
      free(quiz_file->records[i].note);
      quiz_file->records[i].note = NULL;
    }
  }
}

void write_quiz_file(QuizFile quiz_file, FILE *fp)
{
  fwrite(&quiz_file.record_count, sizeof(u16), 1, fp);
  fwrite(&quiz_file.reject_count, sizeof(u16), 1, fp);

  for (int i = 0; i < quiz_file.record_count; i++)
  {
    fwrite(quiz_file.records[i].answers, QUESTION_ARR_SIZE, 1, fp);

    if (quiz_file.records[i].note)
      fwrite(quiz_file.records[i].note, sizeof(char), strlen(quiz_file.records[i].note) + 1, fp);
    else
      fwrite(&(char){0}, sizeof(char), 1, fp);
  }
}

void print_participant(QuizFile quiz_file, int idx)
{
  printf(" -- Participant %hu:\n", idx);

  for (u16 j = 0; j < QUESTION_COUNT; j++)
  {
    int answer_code = (quiz_file.records[idx - 1].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
    char *comment = (
      answer_code == LEFT_CODE ? "Left"
      : answer_code == RIGHT_CODE ? "Right"
      : "ERROR"
    );
    printf("\t -- Question %hu: %s\n", j + 1, questions[j]);
    int ans_idx = j * 2;
    if (answer_code == RIGHT_CODE)
      ans_idx += 1;
    printf("-- Answer: %s (%s)\n\n", answers[ans_idx], comment);
  }

  printf("\t -- Note: %s\n", quiz_file.records[idx - 1].note ? quiz_file.records[idx - 1].note : "None");
}

void print_participants(QuizFile quiz_file)
{
  for (int i = 1 ; i <= quiz_file.record_count; i++)
  {
    print_participant(quiz_file, i);
    puts("\n");
  }
}

void print_quiz_file(QuizFile quiz_file, const char filename[])
{
  printf("\t-- %s --\n\n", filename);
  fputs(" -- Record count: ", stdout);
  printf("%hu\n", quiz_file.record_count);
  fputs(" -- Reject count: ", stdout);
  printf("%hu\n", quiz_file.reject_count);
  print_participants(quiz_file);
  putchar('\n');
}

static inline void print_participant_no_question_texts(QuizFile quiz_file, int idx)
{
  printf(" -- Participant %hu:\n", idx);

  for (u16 j = 0; j < QUESTION_COUNT; j++)
  {
    int answer_code = (quiz_file.records[idx - 1].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
    char *comment = (
      answer_code == LEFT_CODE ? "Left"
      : answer_code == RIGHT_CODE ? "Right"
      : "ERROR"
    );
    int ans_idx = j * 2;
    if (answer_code == RIGHT_CODE)
      ans_idx += 1;
    printf("\t -- Question %hu: %s (%s)\n", j + 1, answers[ans_idx], comment);
  }

  printf("\t -- Note: %s\n", quiz_file.records[idx - 1].note ? quiz_file.records[idx - 1].note : "None");
}

static inline void print_participants_no_question_texts(QuizFile quiz_file)
{
  for (int i = 1 ; i <= quiz_file.record_count; i++)
  {
    print_participant_no_question_texts(quiz_file, i);
    puts("\n");
  }
}

void print_quiz_file_no_question_texts(QuizFile quiz_file, const char filename[])
{
  printf("\t-- %s --\n\n", filename);
  fputs(" -- Record count: ", stdout);
  printf("%hu\n", quiz_file.record_count);
  fputs(" -- Reject count: ", stdout);
  printf("%hu\n", quiz_file.reject_count);
  print_participants_no_question_texts(quiz_file);
  putchar('\n');
}
