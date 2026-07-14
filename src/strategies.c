#include "../include/strategies.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/questions.h"
#include "../include/console.h"
#include "../include/filename.h"
#include "../include/str.h"
#include "../include/quiz_file_funcs.h"

void initialize_file_mode()
{
  clear_console();
  char filename_buf[4096], *filename = get_filename(filename_buf);
  if (filename[0] == '-' && filename[1] == '1')
  {
    clear_console();
    return;
  }

  FILE *fp = fopen(filename, "wb");

  if (!fp)
  {
    file_error();
    return;
  }

  fwrite(&(int){0}, sizeof(u16), 2, fp);
  fclose(fp);
  clear_console();
  printf(" -- Initialised file %s!\n\n", filename);
}

static inline void add_entry(QuizFile *quiz_file)
{
  Record *temp = malloc((quiz_file->record_count + 1) * sizeof(Record));
  
  if (!temp)
  {
    alloc_error();
    return;
  }

  memcpy(temp, quiz_file->records, quiz_file->record_count * sizeof(Record));

  for (int i = 0; i < TEST_QUESTION_COUNT; i++)
  {
    while (1)
    {
      clear_console();
      int answer;
      printf("Test question %d (%d for left, %d for undecided, %d for right, -1 to return)\n", i + 1, LEFT_CODE, UNDECIDED_CODE, RIGHT_CODE);
      printf(" -- Question: %s\n", test_questions[i]);
      fputs(" -- Answer code: ", stdout);
      
      if (scanf("%d", &answer) != 1)
      {
        scanf_fail();
        continue;
      }

      if (answer == -1)
      {
        free(temp);
        return;
      }

      if (answer != LEFT_CODE && answer != RIGHT_CODE && answer != UNDECIDED_CODE)
      {
        invalid_option();
        continue;
      }
  
      temp[quiz_file->record_count].test_answers[i >> 2] &= ~(0x3 << ((i & 0x3) << 1));
      temp[quiz_file->record_count].test_answers[i >> 2] |= answer << ((i & 0x3) << 1);
      break;
    }
  }

  for (int i = 0; i < LONG_QUESTION_COUNT; i++)
  {
    while (1)
    {
      clear_console();
      int answer;
      printf("Long question %d (%d for left, %d for undecided, %d for right, -1 to return)\n", i, LEFT_CODE, UNDECIDED_CODE, RIGHT_CODE);
      printf(" -- Question: %s\n", long_questions[i]);
      fputs(" -- Answer code: ", stdout);
      
      if (scanf("%d", &answer) != 1)
      {
        scanf_fail();
        continue;
      }
  
      temp[quiz_file->record_count].long_verdicts[i >> 2] &= ~(0x3 << ((i & 0x3) << 1));
      temp[quiz_file->record_count].long_verdicts[i >> 2] |= answer << ((i & 0x3) << 1);
  
      fputs(" -- Answer body: ", stdout);
      temp[quiz_file->record_count].long_answers[i] = read_str_dynamic();
      break;
    }
  }

  clear_console();
  puts("Add a note about the participant? (0 for no, 1 for yes)");

  int option;

  if (scanf("%d", &option) != 1)
  {
    scanf_fail();
    return;
  }

  if (option == 1)
  {
    clear_console();
    puts(" -- Enter the note:");
    char *note = read_str_dynamic();
    temp[quiz_file->record_count].note = note;
    clear_console();
  }
  else if (option != 0)
  {
    invalid_option();
  }

  quiz_file->record_count++;
  free(quiz_file->records);
  quiz_file->records = temp;
}

static inline void remove_entry(QuizFile *quiz_file, int idx)
{
  for (int i = 0; i < LONG_QUESTION_COUNT; i++)
  {
    free(quiz_file->records[idx].long_answers[i]);
  }

  free(quiz_file->records[idx].note);
  
  for (int i = idx; i < quiz_file->record_count - 1; i++)
  {
    quiz_file->records[i] = quiz_file->records[i + 1];
  }

  quiz_file->record_count--;

  if (quiz_file->record_count == 0)
  {
    free(quiz_file->records);
    quiz_file->records = NULL;
  }
  else
  {
    Record *temp = realloc(quiz_file->records, quiz_file->record_count * sizeof(Record));
    
    if (temp)
      quiz_file->records = temp;
  }
}

void edit_file_mode()
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
    file_error();
    return;
  }

  puts(" -- TEMP: FILE OPENED!\n");

  QuizFile quiz_file;
  read_quiz_file(&quiz_file, fp);

  fclose(fp);

  clear_console();

  while (1)
  {
    puts("/------------------\n|--EDIT-OPTIONS----\n\\------------------\n");
    printf("\t-- File: %s --\n\n", filename);
    puts(" -- 1. Check file contents.\n -- 2. Add an entry.\n -- 3. Edit an entry\n -- 4. Remove an entry.\n -- -1. Exit edit mode. (CLOSE SUBMENU)");

    int option;
  
    if (scanf("%d", &option) != 1)
    {
      scanf_fail();
      continue;
    }

    putchar('\n');

    if (option == 1)
    {
      clear_console();
      print_quiz_file(quiz_file, filename);
    }
    else if (option == 2)
    {
      clear_console();
      puts("0 to add an entry, 1 to add a reject, -1 to return.");

      if (scanf("%d", &option) != 1)
      {
        scanf_fail();
        continue;
      }
      
      if (option == 0)
      {
        add_entry(&quiz_file);
        fp = fopen(filename, "wb");
        clear_console();

        if (!fp)
        {
          file_error();
          return;
        }

        write_quiz_file(quiz_file, fp);
        fclose(fp);
        puts(" -- Added an entry!\n");
      }
      else if (option == 1)
      {
        quiz_file.reject_count++;
        fp = fopen(filename, "wb");
        clear_console();

        if (!fp)
        {
          file_error();
          return;
        }

        write_quiz_file(quiz_file, fp);
        fclose(fp);
        puts(" -- Added a reject!\n");
      }
      else if (option == -1)
      {
        clear_console();
        continue;
      }
      else
      {
        invalid_option();
        continue;
      }
    }
    else if (option == 3)
    {
      clear_console();
      print_participants(quiz_file);
      puts("Which participant to edit? (-1 to return)");
      
      int participant_idx;

      if (scanf("%d", &participant_idx) != 1)
      {
        scanf_fail();
        continue;
      }

      if (participant_idx == -1)
      {
        clear_console();
        continue;
      }

      if (participant_idx < 1 || participant_idx > quiz_file.record_count)
      {
        invalid_option();
        continue;
      }

      clear_console();
      print_participant(quiz_file, participant_idx);

      puts("Which block to edit? (0 for test, 1 for long, 2 for note, -1 to return)");

      int mode, question_idx;

      if (scanf("%d", &mode) != 1)
      {
        scanf_fail();
        continue;
      }

      if (mode == -1)
      {
        clear_console();
        continue;
      }

      if (mode < 0 || mode > 2)
      {
        invalid_option();
        continue;
      }

      if (mode == 2)
      {
        clear_console();
        puts("Enter note: (-1 to return)");
        char *new_note = read_str_dynamic();

        if (!new_note)
        {
          alloc_error();
          continue;
        }

        if (new_note[0] == '-' && new_note[1] == '1')
        {
          free(new_note);
          continue;
        }

        free(quiz_file.records[participant_idx - 1].note);
        quiz_file.records[participant_idx - 1].note = new_note;
        clear_console();
      }
      else
      {
        puts("Enter the question number: (-1 to return)");
  
        if (scanf("%d", &question_idx) != 1)
        {
          scanf_fail();
          continue;
        }
  
        if (question_idx == -1)
        {
          clear_console();
          continue;
        }
  
        if (question_idx < 1)
        {
          invalid_option();
          continue;
        }
  
        if (mode == 0)
        {
          if (question_idx > TEST_QUESTION_COUNT)
          {
            invalid_option();
            continue;
          }
  
          clear_console();
          int answer;
          printf(
            "Test question %d (%d for left, %d for undecided, %d for right, -1 to return)\n",
            question_idx, LEFT_CODE, UNDECIDED_CODE, RIGHT_CODE
          );
          printf(" -- Question: %s\n", test_questions[question_idx - 1]);
          fputs(" -- Answer code: ", stdout);
          
          if (scanf("%d", &answer) != 1)
          {
            scanf_fail();
            continue;
          }
  
          if (answer == -1)
          {
            clear_console;
            continue;
          }
  
          if (answer != LEFT_CODE && answer != RIGHT_CODE && answer != UNDECIDED_CODE)
          {
            invalid_option();
            continue;
          }
      
          quiz_file.records[participant_idx - 1].test_answers[(question_idx - 1) >> 2] &= ~(0x3 << (((question_idx - 1) & 0x3) << 1));
          quiz_file.records[participant_idx - 1].test_answers[(question_idx - 1) >> 2] |= answer << (((question_idx - 1) & 0x3) << 1);
        }
        else if (mode == 1)
        {
          if (question_idx > LONG_QUESTION_COUNT)
          {
            invalid_option();
            continue;
          }
          
          clear_console();
          int answer;
          printf(
            "Long question %d (%d for left, %d for undecided, %d for right, -1 to return)\n",
            question_idx, LEFT_CODE, UNDECIDED_CODE, RIGHT_CODE
          );
          printf(" -- Question: %s\n", long_questions[question_idx - 1]);
          fputs(" -- Answer code: ", stdout);
          
          if (scanf("%d", &answer) != 1)
          {
            scanf_fail();
            continue;
          }
      
          quiz_file.records[participant_idx - 1].long_verdicts[(question_idx - 1) >> 2] &= ~(0x3 << (((question_idx - 1) & 0x3) << 1));
          quiz_file.records[participant_idx - 1].long_verdicts[(question_idx - 1) >> 2] |= answer << (((question_idx - 1) & 0x3) << 1);
      
          fputs(" -- Answer body: ", stdout);
          quiz_file.records[participant_idx - 1].long_answers[question_idx - 1] = read_str_dynamic();
        }
      }

      FILE *fp = fopen(filename, "wb");

      if (!fp)
      {
        file_error();
        continue;
      }

      write_quiz_file(quiz_file, fp);
      fclose(fp);
      clear_console();
      
      if (mode == 0)
        printf(" -- Updated test question %d on participant %d!\n\n", question_idx, participant_idx);
      else if (mode == 1)
        printf(" -- Updated long question %d on participant %d!\n\n", question_idx, participant_idx);
      else
        printf(" -- Updated note on participant %d!\n\n", participant_idx);
    }
    else if (option == 4)
    {
      clear_console();
      if (quiz_file.record_count <= 0)
      {
        puts("No items in the quiz file!\n");
        continue;
      }
      puts("Which participant to remove? (1-indexed number, 0 to remove reject, -1 to cancel)\n");
      print_quiz_file(quiz_file, filename);
      
      int remove_idx;

      if (scanf("%d", &remove_idx) != 1)
      {
        scanf_fail();
        return;
      }

      if (remove_idx == -1)
        continue;

      if (remove_idx == 0)
      {
        clear_console();

        if (quiz_file.reject_count > 0)
        {
          quiz_file.record_count--;
          puts(" -- Removed a reject!");
        }
        else
        {
          puts(" -- No rejects to remove.");
        }

        continue;
      }
      
      if (remove_idx < 1 || remove_idx >= quiz_file.record_count + 1)
      {
        puts(" -- Index out of range!");
        continue;
      }
      
      remove_entry(&quiz_file, remove_idx - 1);

      fp = fopen(filename, "wb");

      if (!fp)
      {
        file_error();
        return;
      }

      write_quiz_file(quiz_file, fp);
      fclose(fp);

      clear_console();
      printf("Removed participant %d.\n\n", remove_idx);
    }
    else if (option == -1)
    {
      fp = fopen(filename, "wb");
      clear_console();

      if (!fp)
      {
        file_error();
        return;
      }

      write_quiz_file(quiz_file, fp);
      free_quiz_file(quiz_file);
      fclose(fp);
      return;
    }
  }
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
    file_error();
    return;
  }

  QuizFile quiz_file;
  read_quiz_file(&quiz_file, fp);

  fclose(fp);

  clear_console();

  int left_q = 0, right_q = 0, undecided_q = 0;
  int left_participants = 0, right_participants = 0, undecided_participants = 0;
  
  for (int i = 0; i < quiz_file.record_count; i++)
  {
    int personal_left_q = 0, personal_right_q = 0, personal_undecided_q = 0;
    
    for (int j = 0; j < TEST_QUESTION_COUNT; j++)
    {
      int ans_code = (quiz_file.records[i].test_answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;

      if (ans_code == RIGHT_CODE)
        personal_right_q++;
      else if (ans_code == LEFT_CODE)
        personal_left_q++;
      else if (ans_code == UNDECIDED_CODE)
        personal_undecided_q++;
    }

    for (int j = 0; j < LONG_QUESTION_COUNT; j++)
    {
      int ans_code = (quiz_file.records[i].long_verdicts[j >> 2] >> ((j & 0x3) << 1)) & 0x3;

      if (ans_code == RIGHT_CODE)
        personal_right_q++;
      else if (ans_code == LEFT_CODE)
        personal_left_q++;
      else if (ans_code == UNDECIDED_CODE)
        personal_undecided_q++;
    }

    left_q += personal_left_q, right_q += personal_right_q, undecided_q += personal_undecided_q;
    float avg = (float)(
      personal_left_q * LEFT_CODE
      + personal_right_q * RIGHT_CODE
      + personal_undecided_q * UNDECIDED_CODE
    ) / (TEST_QUESTION_COUNT + LONG_QUESTION_COUNT);

    if (avg > (UNDECIDED_CODE + RIGHT_CODE) / 2.f)
      right_participants++;
    if (avg < (LEFT_CODE + UNDECIDED_CODE) / 2.f)
      left_participants++;
    else
      undecided_participants++;
  }


  puts("\t--- TOTAL QUESTION SECTION ---\n");
  
  int total = quiz_file.record_count * (TEST_QUESTION_COUNT + LONG_QUESTION_COUNT);
  printf(" -- Total questions answered: %d\n", total);

  if (total == 0)
  {
    putchar('\n');
    return;
  }
  
  float left_percent = (float)left_q / total * 100.f;
  float right_percent = (float)right_q / total * 100.f;
  float undecided_percent = (float)undecided_q / total * 100.f;
  float avg = (float)(left_q * LEFT_CODE + right_q * RIGHT_CODE + undecided_q * UNDECIDED_CODE) / total;
  printf(" -- L-N-R: %f-%f-%f\n", left_percent, undecided_percent, right_percent);
  printf(" -- Average: %f\n", avg);
  fputs(" -- Winner so far: ", stdout);
  printf(
    "%s\n\n",
    avg > (UNDECIDED_CODE + RIGHT_CODE) / 2.f ? "Right"
    : avg < (LEFT_CODE + UNDECIDED_CODE) / 2.f ? "Left"
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
  undecided_percent = (float)undecided_participants / quiz_file.record_count * 100.f;
  avg = (float)(
    left_participants * LEFT_CODE
    + right_participants * RIGHT_CODE
    + undecided_participants * UNDECIDED_CODE
  ) / quiz_file.record_count;
  printf(" -- L-N-R: %f-%f-%f\n", left_percent, undecided_percent, right_percent);
  printf(" -- Average: %f\n", avg);
  fputs(" -- Winner so far: ", stdout);
  printf(
    "%s\n\n",
    avg > (UNDECIDED_CODE + RIGHT_CODE) / 2.f ? "Right"
    : avg < (LEFT_CODE + UNDECIDED_CODE) / 2.f ? "Left"
    : "Undecided"
  );  
}
