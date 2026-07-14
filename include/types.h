typedef unsigned char u8;
typedef unsigned short u16;

#define TEST_QUESTION_COUNT 2
#define TEST_QUESTION_ARR_SIZE ((TEST_QUESTION_COUNT + 3) / 4)
#define LONG_QUESTION_COUNT 2
#define LONG_QUESTION_ARR_SIZE ((LONG_QUESTION_COUNT + 3) / 4)

typedef enum answer_code
{
  LEFT_CODE = 1,
  UNDECIDED_CODE = 2,
  RIGHT_CODE = 3
} AnswerCode;

typedef struct record
{
  u8 test_answers[TEST_QUESTION_ARR_SIZE];
  u8 long_verdicts[LONG_QUESTION_ARR_SIZE];
  char *long_answers[LONG_QUESTION_COUNT];
  char *note;
} Record;

typedef struct quiz_file
{
  u16 record_count, reject_count;
  Record *records;
} QuizFile;
