#include "../include/quiz_file_funcs.h"
#include <stdlib.h>
#include <string.h>
#include "../include/types.h"
#include "../include/str.h"
#include "../include/console.h"

void free_quiz_file(QuizFile quiz_file)
{
  for (int i = 0; i < quiz_file.record_count; i++)
  {
    for (int j = 0; j < LONG_QUESTION_COUNT; j++)
    {
      free(quiz_file.records[i].long_answers[j]);
    }
    free(quiz_file.records[i].note);
  }

  free(quiz_file.records);
}

void read_quiz_file(QuizFile *quiz_file, FILE *fp)
{
  fread(quiz_file, sizeof(u16), 2, fp);

  quiz_file->records = calloc(quiz_file->record_count, sizeof(Record));

  if (!quiz_file->records)
  {
    alloc_error();
    return;
  }

  for (int i = 0; i < quiz_file->record_count; i++)
  {
    fread(&quiz_file->records[i], TEST_QUESTION_ARR_SIZE + LONG_QUESTION_ARR_SIZE, 1, fp);

    for (int j = 0; j < LONG_QUESTION_COUNT; j++)
    {
      quiz_file->records[i].long_answers[j] = fread_str_dynamic(fp);

      if (!quiz_file->records[i].long_answers[j])
      {
        alloc_error();
        free_quiz_file(*quiz_file);
        return;
      }
    }

    quiz_file->records[i].note = fread_str_dynamic(fp);

    if (!quiz_file->records[i].note)
    {
      alloc_error();
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
  fwrite(&quiz_file, sizeof(u16), 2, fp);

  for (int i = 0; i < quiz_file.record_count; i++)
  {
    fwrite(&quiz_file.records[i], TEST_QUESTION_ARR_SIZE + LONG_QUESTION_ARR_SIZE, 1, fp);

    for (int j = 0; j < LONG_QUESTION_COUNT; j++)
    {
      char *long_ans = quiz_file.records[i].long_answers[j];
      fwrite(long_ans, strlen(long_ans) + 1, 1, fp);
    }

    if (quiz_file.records[i].note)
      fwrite(quiz_file.records[i].note, strlen(quiz_file.records[i].note) + 1, 1, fp);
    else
      fwrite(&(u8){0}, 1, 1, fp);
  }
}

void print_quiz_file(QuizFile quiz_file, const char filename[])
{
  printf("\t-- %s --\n\n", filename);
  fputs(" -- Record count: ", stdout);
  printf("%hu\n", quiz_file.record_count);
  fputs(" -- Reject count: ", stdout);
  printf("%hu\n", quiz_file.reject_count);
  
  for (u16 i = 0; i < quiz_file.record_count; i++)
  {
    printf("\t -- Participant %hu:\n", i + 1);

    for (u16 j = 0; j < TEST_QUESTION_COUNT; j++)
    {
      int answer_code = (quiz_file.records[i].test_answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
      char *comment = (
        answer_code == UNDECIDED_CODE ? "Undecided"
        : answer_code == LEFT_CODE ? "Left"
        : answer_code == RIGHT_CODE ? "Right"
        : "ERROR"
      );
      printf("\t\t -- Test question %hu: %u (%s).\n", j, answer_code, comment);
    }

    for (u16 j = 0; j < LONG_QUESTION_COUNT; j++)
    {
      int answer_code = (quiz_file.records[i].long_verdicts[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
      char *comment = (
        answer_code == UNDECIDED_CODE ? "Undecided"
        : answer_code == LEFT_CODE ? "Left"
        : answer_code == RIGHT_CODE ? "Right"
        : "ERROR"
      );
      printf("\t\t -- Long question %hu: %u (%s). Text: %s\n", j, answer_code, comment, quiz_file.records[i].long_answers[j]);
    }

    printf("\t\t -- Note: %s\n", quiz_file.records[i].note ? quiz_file.records[i].note : "None");
  }

  putchar('\n');
}
