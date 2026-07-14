#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;

#define TEST_QUESTION_COUNT 1
#define TEST_QUESTION_ARR_SIZE ((TEST_QUESTION_COUNT + 3) / 4)
#define LONG_QUESTION_COUNT 1
#define LONG_QUESTION_ARR_SIZE ((LONG_QUESTION_COUNT + 3) / 4)

static char session_filename[4096];

typedef enum answer_code
{
  LEFT_CODE = 1,
  UNDECIDED_CODE = 2,
  RIGHT_CODE = 3
} AnswerCode;

typedef struct record
{
  u8 test_answers[TEST_QUESTION_ARR_SIZE];
  u8 long_verdicts[LONG_QUESTION_ARR_SIZE];
  char *long_answers[LONG_QUESTION_COUNT];
  char *note;
} Record;

typedef struct quiz_file
{
  u16 record_count, reject_count;
  Record *records;
} QuizFile;

static inline void clear_console()
{
  fputs("\033[2J\033[H", stdout);
  fflush(stdout);
}

static inline void scanf_fail()
{
  clear_console();
  puts(" -- SCAN ERROR! Couldn't scan the number.\n");
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

static inline void invalid_option()
{
  puts(" -- Invalid option!\n");
}

static inline void alloc_error()
{
  puts(" -- MEMORY ALLOCATION ERROR! The OS failed a malloc.\n");
}

static inline void file_error()
{
  puts(" -- MEMORY ALLOCATION ERROR! The OS failed a malloc.\n");
}

static void read_string(char str[], int max_size)
{
  int i = 0;
  int temp;

  while ((temp = getchar()) == '\n' || temp == '\r' || temp == ' ');

  str[i++] = temp;
  
  while (i < max_size - 1 && (temp = getchar()) != '\n' && temp != '\r' && temp != EOF)
  {
    str[i] = temp;
    i++;
  }
  str[i] = '\0';
}

static char *read_str_dynamic()
{
  int temp;
  while ((temp = getchar()) == '\n' || temp == '\r' || temp == ' ');
  
  int cur_size = 1, max_size = 10;
  char *str = malloc(max_size);
  str[0] = temp;

  while (1)
  {
    if (cur_size == max_size)
    {
      char *temp = realloc(str, max_size *= 2);

      if (!temp)
      {
        free(str);
        return NULL;
      }

      str = temp;
    }

    if ((str[cur_size++] = getchar()) == '\n')
      break;
  }

  str[cur_size - 1] = '\0';
  char *final_str = realloc(str, cur_size);
  return final_str ? final_str : str;
}

static char *fread_str_dynamic(FILE *fp)
{
  int cur_size = 0, max_size = 10;
  char *str = malloc(max_size);

  while (1)
  {
    if (cur_size == max_size)
    {
      char *temp = realloc(str, max_size *= 2);

      if (!temp)
      {
        free(str);
        return NULL;
      }

      str = temp;
    }

    int temp;
    
    if ((temp = fgetc(fp)) == '\0')
    {
      str[cur_size++] = temp;
      break;
    }
    else if (temp == EOF)
    {
      str[cur_size++] = '\0';
      break;
    }

    str[cur_size++] = temp;
  }

  char *final_str = realloc(str, cur_size);
  return final_str ? final_str : str;
}

static char *get_filename(char stack_buf[])
{
  if (session_filename[0])
  {
    return session_filename;
  }
  else
  {
    puts("Please enter the file name: (-1 to return)");
    read_string(stack_buf, 4096);
    putchar('\n');
    return stack_buf;
  }
}

static void free_quiz_file(QuizFile quiz_file)
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

static void read_quiz_file(QuizFile *quiz_file, FILE *fp)
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

static void write_quiz_file(QuizFile quiz_file, FILE *fp)
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

static void print_quiz_file(QuizFile quiz_file, const char filename[])
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

static inline void initialize_file_mode()
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
  puts("Done!\n");
}

static inline void add_entry(QuizFile *quiz_file)
{
  Record *temp = realloc(quiz_file->records, (quiz_file->record_count + 1) * sizeof(Record));

  if (!temp)
  {
    alloc_error();
    return;
  }

  quiz_file->record_count++;
  quiz_file->records = temp;

  for (int i = 0; i < TEST_QUESTION_COUNT; i++)
  {
    while (1)
    {
      clear_console();
      int answer;
      printf("Test question %d (%d for left, %d for undecided, %d for right)\n", i, LEFT_CODE, UNDECIDED_CODE, RIGHT_CODE);
      fputs(" -- Answer code: ", stdout);
      
      if (scanf("%d", &answer) != 1)
      {
        scanf_fail();
        continue;
      }

      if (answer != LEFT_CODE && answer != RIGHT_CODE && answer != UNDECIDED_CODE)
      {
        invalid_option();
        continue;
      }
  
      quiz_file->records[quiz_file->record_count - 1].test_answers[i >> 2] &= ~(0x3 << ((i & 0x3) << 1));
      quiz_file->records[quiz_file->record_count - 1].test_answers[i >> 2] |= answer << ((i & 0x3) << 1);
      break;
    }
  }

  for (int i = 0; i < LONG_QUESTION_COUNT; i++)
  {
    clear_console();
    int answer;
    printf("Long question %d (%d for left, %d for undecided, %d for right)\n", i, LEFT_CODE, UNDECIDED_CODE, RIGHT_CODE);
    fputs(" -- Answer code: ", stdout);
    
    if (scanf("%d", &answer) != 1)
    {
      quiz_file->record_count--;
      scanf_fail();
      return;
    }

    quiz_file->records[quiz_file->record_count - 1].long_verdicts[i >> 2] &= ~(0x3 << ((i & 0x3) << 1));
    quiz_file->records[quiz_file->record_count - 1].long_verdicts[i >> 2] |= answer << ((i & 0x3) << 1);
    quiz_file->records[quiz_file->record_count - 1].long_answers[i] = read_str_dynamic();
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
    quiz_file->records[quiz_file->record_count - 1].note = note;
    clear_console();
    puts(note ? " -- Note Saved!\n" : "-- Failed to retrieve the note!\n");
  }
  else if (option != 0)
  {
    invalid_option();
  }
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

static inline void edit_file_mode()
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

  while (1)
  {
    puts("/------------------\n|--EDIT-OPTIONS----\n\\------------------\n");
    printf("\t-- File: %s --\n\n", filename);
    puts(" -- 1. Check file contents.\n -- 2. Add an entry.\n -- 3. Remove an entry.\n -- -1. Exit edit mode. (CLOSE SUBMENU)");

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
      }
      else if (option == -1)
      {
        clear_console();
        continue;
      }
      else
      {
        clear_console();
        invalid_option();
        continue;
      }
    }
    else if (option == 3)
    {
      clear_console();
      if (quiz_file.record_count <= 0)
      {
        puts("No items in the quiz file!\n");
        continue;
      }
      puts("Which participant to remove? (1-indexed number, -1 to cancel)\n");
      print_quiz_file(quiz_file, filename);
      
      int remove_idx;

      if (scanf("%d", &remove_idx) != 1)
      {
        scanf_fail();
        return;
      }

      if (remove_idx == -1)
        continue;
      
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

static inline void stat_mode()
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

int main()
{
  session_filename[0] = '\0';
  
  clear_console();
  puts("/-------------------------------\\\n"
       "|---------QUIZ--MANAGER---------|\n"
       "\\-------------------------------/\n");

  while (1)
  {
    puts("\t---  OPTIONS ---\n -- 1. Initialize quiz file.\n -- 2. Edit quiz file. (OPEN SUBMENU)\n -- 3. Stats.");
    if (session_filename[0])
      printf(" -- -1. Clear session filename (%s).\n\n", session_filename);
    else
      puts(" -- -1. Set session filename.\n\n");
    
    int option;
  
    if (scanf("%d", &option) != 1)
    {
      scanf_fail();
      continue;
    }

    if (option == 1)
    {
      initialize_file_mode();
    }
    else if (option == 2)
    {
      edit_file_mode();
    }
    else if (option == 3)
    {
      stat_mode();
    }
    else if (option == -1)
    {
      if (session_filename[0])
      {
        session_filename[0] = '\0';
      }
      else
      {
        clear_console();
        puts("Please enter the file name: (-1 to return)");
        read_string(session_filename, 4096);
        clear_console();

        if (session_filename[0] == '-' && session_filename[1] == '1')
          session_filename[0] = '\0';
      }
    }
    else
    {
      clear_console();
      invalid_option();
    }
  }
  
  return 0;
}
