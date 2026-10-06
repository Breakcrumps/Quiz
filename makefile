CC = gcc
CFLAGS = -O3 -march=native -flto=auto -fno-math-errno -fno-trapping-math -s
CPHONE = clang

TARGET = quiz
TRANSLATOR = translator
SRC = $(wildcard src/*.c)

.PHONY: phone all

all: $(TARGET)

$(TARGET): $(SRC) quiz.c
	$(CC) $(CFLAGS) -fwhole-program $^ -o $(TARGET)
$(TRANSLATOR): $(SRC) translator.c
	$(CC) $(CFLAGS) -fwhole-program $^ -o $(TRANSLATOR)
phone: $(SRC) quiz.c
	$(CPHONE) $(CFLAGS) $^ -o $(TARGET)
