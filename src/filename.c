#include "../include/filename.h"
#include <stdio.h>
#include <string.h>
#include "../include/str.h"

char session_filename[FILENAME_LEN];

char *get_filename(char stack_buf[])
{
  if (session_filename[0])
    return session_filename;

  puts("Please enter the file name: (-1 to return)");
  read_string(stack_buf, 4096);
  putchar('\n');
  return stack_buf;
}

void save_filename(const char *filename)
{
  FILE *fp = fopen("filename.dat", "wb");

  if (!fp)
  {
    puts(" -- Couldn't save session filename to persistent storage!\n");
    return;
  }

  fwrite(session_filename, sizeof(char), strlen(session_filename) + 1, fp);
  fclose(fp);
}
