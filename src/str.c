#include "../include/str.h"
#include <stdlib.h>
#include <stdio.h>

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
