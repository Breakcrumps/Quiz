#include "include/filename.h"
#include "include/modes.h"
#include "include/console.h"
#include "include/str.h"
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
  #ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
  #endif

  setlocale(LC_ALL, ".UTF8");

  FILE *fp = fopen("filename.dat", "rb");

  if (!fp)
  {
    session_filename[0] = '\0';
  }
  else
  {
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fread(session_filename, sizeof(char), file_size, fp);
    fclose(fp);
  }
  
  clear_console();
  puts(
    "/-------------------------------\\\n"
    "|---------QUIZ--MANAGER---------|\n"
    "\\-------------------------------/\n"
  );

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
      report_scanf_fail();
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
      set_clear_session_filename_mode(session_filename);
    }
    else
    {
      clear_console();
      report_invalid_option();
    }
  }
  
  return 0;
}
