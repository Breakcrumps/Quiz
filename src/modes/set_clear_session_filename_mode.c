#include "../../include/modes.h"
#include <stdio.h>
#include "../../include/console.h"
#include "../../include/str.h"

void set_clear_session_filename_mode(char *session_filename)
{
  if (session_filename[0])
  {
    int remove_code = remove("filename.dat");
    session_filename[0] = '\0';
    clear_console();

    if (remove_code != 0)
      puts(" -- Couldn't clear persistent filename cache!\n");
  }
  else
  {
    clear_console();
    puts("Please enter the file name: (-1 to return)");
    read_string(session_filename, 4096);
    clear_console();

    if (session_filename[0] == '-' && session_filename[1] == '1')
    {
      session_filename[0] = '\0';
      return;
    }

    save_filename(session_filename);
  }
}
