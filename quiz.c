#include "include/filename.h"
#include "include/strategies.h"
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
          continue;
        }

        FILE *fp = fopen("filename.dat", "wb");

        if (!fp)
        {
          puts(" -- Couldn't save session filename to persistent storage!\n");
          continue;
        }

        fwrite(session_filename, sizeof(char), strlen(session_filename) + 1, fp);
        fclose(fp);
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
