#include "../../include/modes.h"
#include <stdio.h>
#include <string.h>
#include "../../include/filename.h"
#include "../../include/types.h"
#include "../../include/console.h"

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
    report_file_error();
    return;
  }

  fwrite(&(int){0}, sizeof(u16), 2, fp);
  fclose(fp);
  strcpy(session_filename, filename);
  save_filename(session_filename);
  clear_console();
  printf(" -- Initialised file %s!\n\n", filename);
}
