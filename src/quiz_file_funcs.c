#include "../include/quiz_file_funcs.h"
#include <stdlib.h>
#include <stdio.h>
#include "../include/str.h"
#include "../include/console.h"
#include "../include/questions.h"

static inline void free_quiz_file(QuizFile quiz_file)
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

static inline void print_participant(QuizFile quiz_file, int idx)
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

void print_quiz_file(QuizFile quiz_file, const char filename[])
{
  printf("\t-- %s --\n\n", filename);
  fputs(" -- Record count: ", stdout);
  printf("%hu\n", quiz_file.record_count);
  fputs(" -- Reject count: ", stdout);
  printf("%hu\n", quiz_file.reject_count);
  for (int i = 1 ; i <= quiz_file.record_count; i++)
  {
    print_participant(quiz_file, i);
    puts("\n");
  }
  putchar('\n');
}

void print_global_stats(QuizFile quiz_file)
{
  clear_console();

  int left_q = 0, right_q = 0;
  int left_participants = 0, right_participants = 0;
  
  for (int i = 0; i < quiz_file.record_count; i++)
  {
    int personal_left_q = 0, personal_right_q = 0;
    
    for (int j = 0; j < QUESTION_COUNT; j++)
    {
      int ans_code = (quiz_file.records[i].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;

      if (ans_code == RIGHT_CODE)
        personal_right_q++;
      else if (ans_code == LEFT_CODE)
        personal_left_q++;
    }

    left_q += personal_left_q, right_q += personal_right_q;
    float avg = (float)(
      personal_left_q * LEFT_CODE
      + personal_right_q * RIGHT_CODE
    ) / (QUESTION_COUNT);
    
    if (avg > (LEFT_CODE + RIGHT_CODE) / 2.f)
      right_participants++;
    else
      left_participants++;
  }


  puts("\t--- TOTAL QUESTION SECTION ---\n");
  
  int total = quiz_file.record_count * QUESTION_COUNT;
  printf(" -- Total questions answered: %d\n", total);

  if (total == 0)
  {
    putchar('\n');
    return;
  }
  
  float left_percent = (float)left_q / total * 100.f;
  float right_percent = (float)right_q / total * 100.f;
  float avg = (float)(left_q * LEFT_CODE + right_q * RIGHT_CODE) / total;
  printf(" -- L-R: %f-%f\n", left_percent, right_percent);
  printf(" -- Average: %f\n", avg);
  fputs(" -- Winner so far: ", stdout);
  printf(
    "%s\n\n",
    avg > (RIGHT_CODE + LEFT_CODE) / 2.f ? "Right"
    : avg < (RIGHT_CODE + LEFT_CODE) / 2.f ? "Left"
    : "Undecided"
  );

  puts("\t--- PER PARTICIPANT SECTION ---\n");

  printf(" -- Total cooperative participants: %d\n", quiz_file.record_count);

  if (quiz_file.record_count == 0)
  {
    putchar('\n');
    return;
  }

  left_percent = (float)left_participants / quiz_file.record_count * 100.f;
  right_percent = (float)right_participants / quiz_file.record_count * 100.f;
  avg = (float)(
    left_participants * LEFT_CODE
    + right_participants * RIGHT_CODE
  ) / quiz_file.record_count;
  printf(" -- L-R: %f-%f\n", left_percent, right_percent);
  printf(" -- Average: %f\n", avg);
  fputs(" -- Winner so far: ", stdout);
  printf(
    "%s\n\n\n",
    avg > (RIGHT_CODE + LEFT_CODE) / 2.f ? "Right"
    : avg < (RIGHT_CODE + LEFT_CODE) / 2.f ? "Left"
    : "Undecided"
  );
}

void print_per_question_stats(QuizFile quiz_file)
{
  clear_console();
  
  puts("\t--- PER QUESTION STATS ---\n");

  for (int j = 0; j < QUESTION_COUNT; j++)
  {
    int left_votes = 0, right_votes = 0, total_votes = 0;

    for (int i = 0; i < quiz_file.record_count; i++)
    {
      int ans_code = (quiz_file.records[i].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
      if (ans_code == LEFT_CODE)
        left_votes++;
      else if (ans_code == RIGHT_CODE)
        right_votes++;
      total_votes++;
    }

    printf("Question %d: %s\n", j + 1, questions[j]);

    if (total_votes == 0)
    {
      puts(" -- No answers recorded.\n");
      continue;
    }

    float left_p = (float)left_votes / total_votes * 100.f;
    float right_p = (float)right_votes / total_votes * 100.f;
    char *winner = (left_votes > right_votes) ? "Left" : (right_votes > left_votes) ? "Right" : "Tie";

    printf(" -- Breakdown (L / R): %.1f%% / %.1f%%  (%d vs %d)\n", left_p, right_p, left_votes, right_votes);
    printf(" -- Majority stance: %s\n\n", winner);
  }

  putchar('\n');
}

void print_per_participant_stats(QuizFile quiz_file)
{
  clear_console();

  if (quiz_file.record_count == 0)
  {
    puts("No entries in the file!\n");
    return;
  }
  
  puts("\t--- PARTICIPANT PROFILE ANALYSIS ---\n");

  for (int i = 0; i < quiz_file.record_count; i++)
  {
    int personal_left_q = 0, personal_right_q = 0;
    
    for (int j = 0; j < QUESTION_COUNT; j++)
    {
      int ans_code = (quiz_file.records[i].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
      if (ans_code == LEFT_CODE)
        personal_left_q++;
      else if (ans_code == RIGHT_CODE)
        personal_right_q++;
    }

    printf(" -- Participant %d:\n", i + 1);

    float left_p = (float)personal_left_q / QUESTION_COUNT * 100.f;
    float right_p = (float)personal_right_q / QUESTION_COUNT * 100.f;

    char *stance = personal_left_q > personal_right_q ? "Left" : personal_left_q < personal_right_q ? "Right" : "Neutral";

    printf(" -- Political Balance (L / R): %.1f%% / %.1f%%  (%d vs %d)\n", left_p, right_p, personal_left_q, personal_right_q);
    printf(" -- Personal Verdict: %s\n", stance);
    printf(" -- Note: %s\n\n", quiz_file.records[i].note ? quiz_file.records[i].note : "None");
  }

  putchar('\n');
}
