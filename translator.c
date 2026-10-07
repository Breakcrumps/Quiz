#include "include/types.h"
#include "include/questions.h"
#include "include/console.h"
#include "include/quiz_file_funcs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

typedef struct extract
{
  unsigned char packed[QUESTION_ARR_SIZE];
  char *note;
} Extract;

static inline void pack_answers(char row[QUESTION_COUNT][128], unsigned char packed[])
{
  memset(packed, 0, QUESTION_ARR_SIZE);

  for (int i = 0; i < QUESTION_COUNT - 1; i++)
  {
    if (i == QUESTION_COUNT - 2)
    {
      if (strstr(row[i], "Нет"))
      {
        packed[byte_idx(i)] |= (RIGHT_CODE << byte_pos(i));
        packed[byte_idx(i + 1)] |= (RIGHT_CODE << byte_pos(i + 1));
      }
      else if (strstr(row[i], "оно"))
      {
        packed[byte_idx(i)] |= (RIGHT_CODE << byte_pos(i));
        packed[byte_idx(i + 1)] |= (LEFT_CODE << byte_pos(i + 1));
      }
      else
      {
        packed[byte_idx(i)] |= (LEFT_CODE << byte_pos(i));
        packed[byte_idx(i + 1)] |= (LEFT_CODE << byte_pos(i + 1));
      }
      break;
    }

    const char *left_match = answers[i << 1];
    const char *right_match = answers[(i << 1) + 1];

    int code = 
      !strcmp(row[i], left_match) ? LEFT_CODE
      : !strcmp(row[i], right_match) ? RIGHT_CODE
      : 0;

    packed[byte_idx(i)] |= code << byte_pos(i);
  }
}

int main(int argc, char *argv[])
{
  #ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  #endif

  setlocale(LC_ALL, ".UTF8");
  
  if (argc < 3)
  {
    printf("Usage: %s <input_tsv> <output_binary>\n", argv[0]);
    return -1;
  }

  FILE *tsv_fp = fopen(argv[1], "r");

  if (!tsv_fp)
  {
    perror("Error opening TSV file");
    return -1;
  }

  unsigned short capacity = 10, record_count = 0;
  Extract *extracts = malloc(capacity * sizeof(Extract));

  if (!extracts)
  {
    report_alloc_error();
    fclose(tsv_fp);
    return 1;
  }

  char line[8192];

  if (!fgets(line, sizeof(line), tsv_fp))
  {
    fclose(tsv_fp);
    free(extracts);
    return -1;
  }

  while (fgets(line, sizeof(line), tsv_fp))
  {
    char row_answers[QUESTION_COUNT][128], note[256] = "";
    char *token = strtok(line, "\t");
    int col_idx = 0;

    while (token)
    {
      if (col_idx >= 1 && col_idx < QUESTION_COUNT)
      {
        strncpy(row_answers[col_idx - 1], token, 127);
        row_answers[col_idx - 1][127] = '\0';
      }
      else if (col_idx == QUESTION_COUNT)
      {
        strncpy(note, token, 255);
        note[255] = '\0';
      }
      token = strtok(NULL, "\t");
      col_idx++;
    }

    if (record_count >= capacity)
    {
      extracts = realloc(extracts, (capacity *= 2) * sizeof(Extract));

      if (!extracts)
      {
        report_alloc_error();
        fclose(tsv_fp);
        return 1;
      }
    }

    pack_answers(row_answers, extracts[record_count].packed);
    extracts[record_count].note = strdup(note);
    record_count++;
  }

  fclose(tsv_fp);

  FILE *bin_fp = fopen(argv[2], "wb");

  if (!bin_fp)
  {
    perror("Error opening binary file");
    for (int i = 0; i < record_count; i++)
    {
      free(extracts[i].note);
    }
    free(extracts);
    return 1;
  }

  fwrite(&record_count, sizeof(unsigned short), 1, bin_fp);
  fwrite(&(unsigned short){0}, sizeof(unsigned short), 1, bin_fp);

  for (int i = 0; i < record_count; i++)
  {
    fwrite(extracts[i].packed, 1, QUESTION_ARR_SIZE, bin_fp);
    fwrite(extracts[i].note, 1, strlen(extracts[i].note) + 1, bin_fp);
    free(extracts[i].note);
  }

  fclose(bin_fp);
  free(extracts);

  printf("Готово! Сконвертировано записей: %d в файл '%s'.\n", record_count, argv[2]);
  return 0;
}
