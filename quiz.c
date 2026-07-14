#include "include/filename.h"
#include "include/strategies.h"
#include "include/console.h"
#include "include/str.h"

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
      clear_console();
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

        if (session_filename[0] == '-' && session_filename[1] == '1')
          session_filename[0] = '\0';
      }

      clear_console();
    }
    else
    {
      clear_console();
      invalid_option();
    }
  }
  
  return 0;
}
