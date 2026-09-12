#include "../include/modes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/questions.h"
#include "../include/console.h"
#include "../include/filename.h"
#include "../include/str.h"
#include "../include/quiz_file_funcs.h"

// void initialize_file_mode()
// {
//   clear_console();

//   char filename_buf[4096], *filename = get_filename(filename_buf);

//   if (filename[0] == '-' && filename[1] == '1')
//   {
//     clear_console();
//     return;
//   }

//   FILE *fp = fopen(filename, "wb");

//   if (!fp)
//   {
//     report_file_error();
//     return;
//   }

//   fwrite(&(int){0}, sizeof(u16), 2, fp);
//   fclose(fp);
//   strcpy(session_filename, filename);
//   save_filename(session_filename);
//   clear_console();
//   printf(" -- Initialised file %s!\n\n", filename);
// }

// static void prompt_question(int idx)
// {
//   printf("Question %d\n", idx + 1);
//   printf(" -- Question: %s\n -- %d: %s\n -- %d: %s\n",
//     questions[idx],
//     LEFT_CODE, answers[idx * 2],
//     RIGHT_CODE, answers[idx * 2 + 1]
//   );
//   fputs(" -- Answer code: ", stdout);
// }

// static inline int add_entry(QuizFile *quiz_file)
// {
//   Record *temp = malloc((quiz_file->record_count + 1) * sizeof(Record));
  
//   if (!temp)
//   {
//     report_alloc_error();
//     return 1;
//   }

//   memcpy(temp, quiz_file->records, quiz_file->record_count * sizeof(Record));

//   int i;

//   for (i = 0; i < QUESTION_COUNT; i++)
//   {
//     while (1)
//     {
//       clear_console();
//       int answer;
//       prompt_question(i);
      
//       if (scanf("%d", &answer) != 1)
//       {
//         report_scanf_fail();
//         continue;
//       }

//       if (answer == -1)
//       {
//         clear_console();
//         free(temp);
//         return 1;
//       }

//       if (answer == -2)
//       {
//         i--;
//         if (i > -1)
//           i--;
//         break;
//       }

//       if (answer != LEFT_CODE && answer != RIGHT_CODE)
//       {
//         report_invalid_option();
//         continue;
//       }
  
//       temp[quiz_file->record_count].answers[i >> 2] &= ~(0x3 << ((i & 0x3) << 1));
//       temp[quiz_file->record_count].answers[i >> 2] |= answer << ((i & 0x3) << 1);
//       break;
//     }
//   }

//   clear_console();
//   puts("Add a note about the participant? (0 for no, 1 for yes)");

//   int option;

//   while (scanf("%d", &option) != 1)
//   {
//     report_scanf_fail();
//   }

//   if (option == 1)
//   {
//     clear_console();
//     puts(" -- Enter the note:");
//     char *note = read_str_dynamic();
//     temp[quiz_file->record_count].note = note;
//     clear_console();
//   }
//   else if (option != 0)
//   {
//     report_invalid_option();
//   }

//   quiz_file->record_count++;
//   free(quiz_file->records);
//   quiz_file->records = temp;
//   return 0;
// }

// static inline void remove_entry(QuizFile *quiz_file, int idx)
// {
//   free(quiz_file->records[idx].note);
  
//   for (int i = idx; i < quiz_file->record_count - 1; i++)
//   {
//     quiz_file->records[i] = quiz_file->records[i + 1];
//   }

//   quiz_file->record_count--;

//   if (quiz_file->record_count == 0)
//   {
//     free(quiz_file->records);
//     quiz_file->records = NULL;
//   }
//   else
//   {
//     Record *temp = realloc(quiz_file->records, quiz_file->record_count * sizeof(Record));
    
//     if (temp)
//       quiz_file->records = temp;
//   }
// }

// static inline void add_entry_submode(QuizFile *quiz_file, const char *filename)
// {
//   clear_console();
//   puts("0 to add an entry, 1 to add a reject, anything else to return.");

//   int option;

//   if (scanf("%d", &option) != 1)
//   {
//     report_scanf_fail();
//     return;
//   }
  
//   if (option == 0)
//   {
//     if (add_entry(quiz_file))
//       return;

//     FILE *fp = fopen(filename, "wb");
    
//     if (!fp)
//     {
//       report_file_error();
//       return;
//     }

//     clear_console();
//     write_quiz_file(*quiz_file, fp);
//     fclose(fp);
//     puts(" -- Added an entry!\n");
//   }
//   else if (option == 1)
//   {
//     quiz_file->reject_count++;
//     FILE *fp = fopen(filename, "wb");
//     clear_console();

//     if (!fp)
//     {
//       report_file_error();
//       return;
//     }

//     write_quiz_file(*quiz_file, fp);
//     fclose(fp);
//     puts(" -- Added a reject!\n");
//   }
//   else
//   {
//     clear_console();
//   }
// }

// static inline void edit_entry_submode(QuizFile *quiz_file, const char *filename)
// {
//   clear_console();

//   if (!quiz_file->record_count)
//   {
//     puts("No records in the quiz file!\n");
//     return;
//   }
  
//   print_participants(*quiz_file);
//   puts("\nWhich participant to edit? (-1 to return)");
  
//   int participant_idx;

//   if (scanf("%d", &participant_idx) != 1)
//   {
//     report_scanf_fail();
//     return;
//   }

//   if (participant_idx == -1)
//   {
//     clear_console();
//     return;
//   }

//   if (participant_idx < 1 || participant_idx > quiz_file->record_count)
//   {
//     report_invalid_option();
//     return;
//   }

//   clear_console();
//   print_participant(*quiz_file, participant_idx);

//   puts("\nWhich block to edit?\n -- 0: Questions\n -- 1: Note\n -- Anything else: Return");

//   int mode, question_idx;

//   if (scanf("%d", &mode) != 1)
//   {
//     report_scanf_fail();
//     return;
//   }

//   if (mode == 1)
//   {
//     clear_console();
//     puts("Enter note: (-1 to return)");
//     char *new_note = read_str_dynamic();

//     if (!new_note)
//     {
//       report_alloc_error();
//       return;
//     }

//     if (new_note[0] == '-' && new_note[1] == '1')
//     {
//       free(new_note);
//       return;
//     }

//     free(quiz_file->records[participant_idx - 1].note);
//     quiz_file->records[participant_idx - 1].note = new_note;
//     clear_console();
//   }
//   else if (mode == 0)
//   {
//     puts("\nEnter the question number: (-1 to return)");

//     if (scanf("%d", &question_idx) != 1)
//     {
//       report_scanf_fail();
//       return;
//     }

//     if (question_idx == -1)
//     {
//       clear_console();
//       return;
//     }

//     if (question_idx < 1)
//     {
//       report_invalid_option();
//       return;
//     }

//     if (question_idx > QUESTION_COUNT)
//     {
//       report_invalid_option();
//       return;
//     }

//     clear_console();
//     int answer;
//     prompt_question(question_idx - 1);
    
//     if (scanf("%d", &answer) != 1)
//     {
//       report_scanf_fail();
//       return;
//     }

//     if (answer == -1)
//     {
//       clear_console();
//       return;
//     }

//     if (answer != LEFT_CODE && answer != RIGHT_CODE)
//     {
//       report_invalid_option();
//       return;
//     }

//     quiz_file->records[participant_idx - 1].answers[(question_idx - 1) >> 2] &= ~(0x3 << (((question_idx - 1) & 0x3) << 1));
//     quiz_file->records[participant_idx - 1].answers[(question_idx - 1) >> 2] |= answer << (((question_idx - 1) & 0x3) << 1);
//   }
//   else
//   {
//     clear_console();
//     return;
//   }

//   FILE *fp = fopen(filename, "wb");

//   if (!fp)
//   {
//     report_file_error();
//     return;
//   }

//   write_quiz_file(*quiz_file, fp);
//   fclose(fp);
//   clear_console();
  
//   if (mode == 0)
//     printf(" -- Updated question %d on participant %d!\n\n", question_idx, participant_idx);
//   else
//     printf(" -- Updated note on participant %d!\n\n", participant_idx);
// }

// static inline void remove_entry_submode(QuizFile *quiz_file, const char *filename)
// {
//   clear_console();

//   if (quiz_file->record_count <= 0)
//   {
//     puts("No items in the quiz file!\n");
//     return;
//   }
//   print_quiz_file_no_question_texts(*quiz_file, filename);
//   puts("Which participant to remove? (1-indexed, 0 to remove a reject, -1 to cancel)\n");
  
//   int remove_idx;

//   if (scanf("%d", &remove_idx) != 1)
//   {
//     report_scanf_fail();
//     return;
//   }

//   if (remove_idx == -1)
//     return;

//   if (remove_idx == 0)
//   {
//     clear_console();

//     if (quiz_file->reject_count > 0)
//     {
//       quiz_file->reject_count--;

//       FILE *fp = fopen(filename, "wb");

//       if (!fp)
//       {
//         report_file_error();
//         return;
//       }

//       write_quiz_file(*quiz_file, fp);
//       fclose(fp);
      
//       puts(" -- Removed a reject!");
//     }
//     else
//     {
//       puts(" -- No rejects to remove.");
//     }

//     return;
//   }
  
//   if (remove_idx < 1 || remove_idx >= quiz_file->record_count + 1)
//   {
//     puts(" -- Index out of range!");
//     return;
//   }
  
//   remove_entry(quiz_file, remove_idx - 1);

//   FILE *fp = fopen(filename, "wb");

//   if (!fp)
//   {
//     report_file_error();
//     return;
//   }

//   write_quiz_file(*quiz_file, fp);
//   fclose(fp);

//   clear_console();
//   printf("Removed participant %d.\n\n", remove_idx);
// }

// void edit_file_mode()
// {
//   clear_console();

//   char filename_buf[4096], *filename = get_filename(filename_buf);

//   if (filename[0] == '-' && filename[1] == '1')
//   {
//     clear_console();
//     return;
//   }

//   FILE *fp = fopen(filename, "rb");

//   if (!fp)
//   {
//     report_file_error();
//     return;
//   }

//   puts(" -- TEMP: FILE OPENED! READING QUIZ FILE.\n");

//   QuizFile quiz_file;
//   read_quiz_file(&quiz_file, fp);

//   fclose(fp);

//   clear_console();

//   while (1)
//   {
//     puts("/------------------\n|--EDIT-OPTIONS----\n\\------------------\n");
//     printf("\t-- File: %s --\n\n", filename);
//     puts(
//       " -- 1. Check file contents.\n"
//       " -- 2. Add an entry.\n"
//       " -- 3. Edit an entry.\n"
//       " -- 4. Remove an entry.\n"
//       " -- -1. Exit edit mode. (CLOSE SUBMENU)"
//     );

//     int option;
  
//     if (scanf("%d", &option) != 1)
//     {
//       report_scanf_fail();
//       continue;
//     }

//     putchar('\n');

//     if (option == 1)
//     {
//       clear_console();
//       print_quiz_file(quiz_file, filename);
//     }
//     else if (option == 2)
//     {
//       add_entry_submode(&quiz_file, filename);
//     }
//     else if (option == 3)
//     {
//       edit_entry_submode(&quiz_file, filename);
//     }
//     else if (option == 4)
//     {
//       remove_entry_submode(&quiz_file, filename);
//     }
//     else if (option == -1)
//     {
//       fp = fopen(filename, "wb");
//       clear_console();

//       if (!fp)
//       {
//         report_file_error();
//         return;
//       }

//       write_quiz_file(quiz_file, fp);
//       free_quiz_file(quiz_file);
//       fclose(fp);
//       return;
//     }
//   }
// }

// static inline void print_global_stats(QuizFile quiz_file)
// {
//   clear_console();

//   int left_q = 0, right_q = 0;
//   int left_participants = 0, right_participants = 0;
  
//   for (int i = 0; i < quiz_file.record_count; i++)
//   {
//     int personal_left_q = 0, personal_right_q = 0;
    
//     for (int j = 0; j < QUESTION_COUNT; j++)
//     {
//       int ans_code = (quiz_file.records[i].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;

//       if (ans_code == RIGHT_CODE)
//         personal_right_q++;
//       else if (ans_code == LEFT_CODE)
//         personal_left_q++;
//     }

//     left_q += personal_left_q, right_q += personal_right_q;
//     float avg = (float)(
//       personal_left_q * LEFT_CODE
//       + personal_right_q * RIGHT_CODE
//     ) / (QUESTION_COUNT);
    
//     if (avg > (LEFT_CODE + RIGHT_CODE) / 2.f)
//       right_participants++;
//     else
//       left_participants++;
//   }


//   puts("\t--- TOTAL QUESTION SECTION ---\n");
  
//   int total = quiz_file.record_count * QUESTION_COUNT;
//   printf(" -- Total questions answered: %d\n", total);

//   if (total == 0)
//   {
//     putchar('\n');
//     return;
//   }
  
//   float left_percent = (float)left_q / total * 100.f;
//   float right_percent = (float)right_q / total * 100.f;
//   float avg = (float)(left_q * LEFT_CODE + right_q * RIGHT_CODE) / total;
//   printf(" -- L-R: %f-%f\n", left_percent, right_percent);
//   printf(" -- Average: %f\n", avg);
//   fputs(" -- Winner so far: ", stdout);
//   printf(
//     "%s\n\n",
//     avg > (RIGHT_CODE + LEFT_CODE) / 2.f ? "Right"
//     : avg < (RIGHT_CODE + LEFT_CODE) / 2.f ? "Left"
//     : "Undecided"
//   );

//   puts("\t--- PER PARTICIPANT SECTION ---\n");

//   printf(" -- Total cooperative participants: %d\n", quiz_file.record_count);

//   if (quiz_file.record_count == 0)
//   {
//     putchar('\n');
//     return;
//   }

//   left_percent = (float)left_participants / quiz_file.record_count * 100.f;
//   right_percent = (float)right_participants / quiz_file.record_count * 100.f;
//   avg = (float)(
//     left_participants * LEFT_CODE
//     + right_participants * RIGHT_CODE
//   ) / quiz_file.record_count;
//   printf(" -- L-R: %f-%f\n", left_percent, right_percent);
//   printf(" -- Average: %f\n", avg);
//   fputs(" -- Winner so far: ", stdout);
//   printf(
//     "%s\n\n\n",
//     avg > (RIGHT_CODE + LEFT_CODE) / 2.f ? "Right"
//     : avg < (RIGHT_CODE + LEFT_CODE) / 2.f ? "Left"
//     : "Undecided"
//   );
// }

// static inline void print_per_question_stats(QuizFile quiz_file)
// {
//   clear_console();
  
//   puts("\t--- PER QUESTION STATS ---\n");

//   for (int j = 0; j < QUESTION_COUNT; j++)
//   {
//     int left_votes = 0, right_votes = 0, total_votes = 0;

//     for (int i = 0; i < quiz_file.record_count; i++)
//     {
//       int ans_code = (quiz_file.records[i].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
//       if (ans_code == LEFT_CODE)
//         left_votes++;
//       else if (ans_code == RIGHT_CODE)
//         right_votes++;
//       total_votes++;
//     }

//     printf("Question %d: %s\n", j + 1, questions[j]);

//     if (total_votes == 0)
//     {
//       puts(" -- No answers recorded.\n");
//       continue;
//     }

//     float left_p = (float)left_votes / total_votes * 100.f;
//     float right_p = (float)right_votes / total_votes * 100.f;
//     char *winner = (left_votes > right_votes) ? "Left" : (right_votes > left_votes) ? "Right" : "Tie";

//     printf(" -- Breakdown (L / R): %.1f%% / %.1f%%  (%d vs %d)\n", left_p, right_p, left_votes, right_votes);
//     printf(" -- Majority stance: %s\n\n", winner);
//   }

//   putchar('\n');
// }

// static inline void print_per_participant_stats(QuizFile quiz_file)
// {
//   clear_console();

//   if (quiz_file.record_count == 0)
//   {
//     puts("No entries in the file!\n");
//     return;
//   }
  
//   puts("\t--- PARTICIPANT PROFILE ANALYSIS ---\n");

//   for (int i = 0; i < quiz_file.record_count; i++)
//   {
//     int personal_left_q = 0, personal_right_q = 0;
    
//     for (int j = 0; j < QUESTION_COUNT; j++)
//     {
//       int ans_code = (quiz_file.records[i].answers[j >> 2] >> ((j & 0x3) << 1)) & 0x3;
//       if (ans_code == LEFT_CODE)
//         personal_left_q++;
//       else if (ans_code == RIGHT_CODE)
//         personal_right_q++;
//     }

//     printf(" -- Participant %d:\n", i + 1);

//     float left_p = (float)personal_left_q / QUESTION_COUNT * 100.f;
//     float right_p = (float)personal_right_q / QUESTION_COUNT * 100.f;

//     char *stance = personal_left_q > personal_right_q ? "Left" : personal_left_q < personal_right_q ? "Right" : "Neutral";

//     printf(" -- Political Balance (L / R): %.1f%% / %.1f%%  (%d vs %d)\n", left_p, right_p, personal_left_q, personal_right_q);
//     printf(" -- Personal Verdict: %s\n", stance);
//     printf(" -- Note: %s\n\n", quiz_file.records[i].note ? quiz_file.records[i].note : "None");
//   }

//   putchar('\n');
// }

// void stat_mode()
// {
//   clear_console();

//   char filename_buf[4096], *filename = get_filename(filename_buf);

//   if (filename[0] == '-' && filename[1] == '1')
//   {
//     clear_console();
//     return;
//   }

//   FILE *fp = fopen(filename, "rb");

//   if (!fp)
//   {
//     report_file_error();
//     return;
//   }

//   QuizFile quiz_file;
//   read_quiz_file(&quiz_file, fp);

//   fclose(fp);

//   while (true)
//   {
//     puts("------------> Stat Mode <------------\n");
//     puts(
//       " -- 1. Stats accumulated from all questions and stats accumulated from every participant.\n"
//       " -- 2. Per-question stats.\n"
//       " -- 3. Per-participant stats.\n"
//       " -- -1. Exit edit mode. (CLOSE SUBMENU)\n"
//     );

//     int option;

//     if (scanf("%d", &option) != 1)
//     {
//       report_scanf_fail();
//       continue;
//     }

//     if (option == -1)
//     {
//       clear_console();
//       return;
//     } 

//     if (option == 1)
//       print_global_stats(quiz_file);
//     else if (option == 2)
//       print_per_question_stats(quiz_file);
//     else if (option == 3)
//       print_per_participant_stats(quiz_file);
//   }
// }

// void set_clear_session_filename_mode(char *session_filename)
// {
//   if (session_filename[0])
//   {
//     int remove_code = remove("filename.dat");
//     session_filename[0] = '\0';
//     clear_console();

//     if (remove_code != 0)
//       puts(" -- Couldn't clear persistent filename cache!\n");
//   }
//   else
//   {
//     clear_console();
//     puts("Please enter the file name: (-1 to return)");
//     read_string(session_filename, 4096);
//     clear_console();

//     if (session_filename[0] == '-' && session_filename[1] == '1')
//     {
//       session_filename[0] = '\0';
//       return;
//     }

//     save_filename(session_filename);
//   }
// }
