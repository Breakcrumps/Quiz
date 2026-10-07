typedef unsigned char u8;
typedef unsigned short u16;

#define QUESTION_COUNT 39
#define QUESTION_ARR_SIZE ((QUESTION_COUNT + 7) / 8)

typedef enum answer_code
{
  LEFT_CODE = 0,
  RIGHT_CODE = 1
} AnswerCode;

typedef struct record
{
  u8 answers[QUESTION_ARR_SIZE];
  char *note;
} Record;

typedef struct quiz_file
{
  u16 record_count, reject_count;
  Record *records;
} QuizFile;
