CC = gcc
CFLAGS = -O3 -march=x86-64-v3 -flto=auto -fno-math-errno -fno-trapping-math -s
CPHONE = clang
CPHONEFLAGS = -O3 -march=armv8-a -flto=auto -fno-math-errno -fno-trapping-math -s

TARGET = quiz
SRC = $(wildcard src/*)

.PHONY: phone all

all: $(TARGET)

$(TARGET): $(SRC) quiz.c
	$(CC) $(CFLAGS) -fwhole-program $^ -o $(TARGET)
phone: $(SRC) quiz.c
	$(CPHONE) $(CPHONEFLAGS) $^ -o $(TARGET)
