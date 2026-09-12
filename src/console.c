#include "../include/console.h"
#include <stdio.h>

void clear_console()
{
  fputs("\033[2J\033[H", stdout);
  fflush(stdout);
}

void report_scanf_fail()
{
  clear_console();
  puts(" -- SCAN ERROR! Couldn't scan the number.\n");
  int c;
  while ((c = getchar()) != '\n' && c != EOF);
}

void report_invalid_option()
{
  clear_console();
  puts(" -- Invalid option!\n");
}

void report_alloc_error()
{
  clear_console();
  puts(" -- MEMORY ALLOCATION ERROR! The OS failed a malloc.\n");
}

void report_file_error()
{
  clear_console();
  puts(" -- FILE OPEN ERROR! Please check if the specified file exists.\n");
}
