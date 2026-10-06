#include "include/console.h"
#include "include/quiz_file_funcs.h"
#include "include/types.h"
#include <locale.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <quiz file>\n", argv[0]);
    return 0;
  }

  FILE *init_fp = fopen(argv[1], "rb");

  if (!init_fp)
  {
    report_file_error();
    return 0;
  }

  fclose(init_fp);
  
  #ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  #endif

  setlocale(LC_ALL, ".UTF8");
  
  clear_console();
  puts(
    "/-------------------------------\\\n"
    "|---------QUIZ--MANAGER---------|\n"
    "\\-------------------------------/\n"
  );

  while (1)
  {
    printf(" <<<  File: %s  >>>\n\n", argv[1]);
    puts(
      "\t---  OPTIONS ---\n"
      " -- 1. Check file contents.\n"
      " -- 2. Stats accumulated from all questions and stats accumulated from every participant.\n"
      " -- 3. Per-question stats.\n"
      " -- 4. Per-participant stats."
    );
    
    int option;
  
    if (scanf("%d", &option) != 1)
    {
      clear_console();
      report_scanf_fail();
      continue;
    }

    FILE *fp = fopen(argv[1], "rb");

    if (!fp)
    {
      report_file_error();
      return 0;
    }

    QuizFile quiz_file;
    read_quiz_file(&quiz_file, fp);
    fclose(fp);

    if (option == 1)
    {
      clear_console();
      print_quiz_file(quiz_file, argv[1]);
    }
    else if (option == 2)
    {
      print_global_stats(quiz_file);
    }
    else if (option == 3)
    {
      print_per_question_stats(quiz_file);
    }
    else if (option == 4)
    {
      print_per_participant_stats(quiz_file);
    }
    else
    {
      clear_console();
      report_invalid_option();
    }
  }
  
  return 0;
}
