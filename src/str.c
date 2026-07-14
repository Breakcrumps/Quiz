#include "../include/str.h"
#include <stdlib.h>

void read_string(char str[], int max_size)
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

char *read_str_dynamic()
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

char *fread_str_dynamic(FILE *fp)
{
  int cur_size = 0, max_size = 10;
  char *str = malloc(max_size);

  while (1)
  {
    if (cur_size == max_size)
    {
      char *temp = realloc(str, (max_size *= 2) * sizeof(char));

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
