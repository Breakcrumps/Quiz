#include "../../include/modes.h"
#include <stdio.h>
#include "../../include/types.h"
#include "../../include/questions.h"

static inline void print_global_stats(QuizFile quiz_file)
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

static inline void print_per_question_stats(QuizFile quiz_file)
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

static inline void print_per_participant_stats(QuizFile quiz_file)
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

void stat_mode()
{
  clear_console();

  char filename_buf[4096], *filename = get_filename(filename_buf);

  if (filename[0] == '-' && filename[1] == '1')
  {
    clear_console();
    return;
  }

  FILE *fp = fopen(filename, "rb");

  if (!fp)
  {
    report_file_error();
    return;
  }

  QuizFile quiz_file;
  read_quiz_file(&quiz_file, fp);

  fclose(fp);

  while (true)
  {
    puts("------------> Stat Mode <------------\n");
    puts(
      " -- 1. Stats accumulated from all questions and stats accumulated from every participant.\n"
      " -- 2. Per-question stats.\n"
      " -- 3. Per-participant stats.\n"
      " -- -1. Exit edit mode. (CLOSE SUBMENU)\n"
    );

    int option;

    if (scanf("%d", &option) != 1)
    {
      report_scanf_fail();
      continue;
    }

    if (option == -1)
    {
      clear_console();
      return;
    } 

    if (option == 1)
      print_global_stats(quiz_file);
    else if (option == 2)
      print_per_question_stats(quiz_file);
    else if (option == 3)
      print_per_participant_stats(quiz_file);
  }
}
