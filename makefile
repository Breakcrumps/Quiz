SHELL := powershell.exe
.SHELLFLAGS := -NoProfile -Command

CC = gcc
CFLAGS = -O3 -march=x86-64-v3 -flto=auto -fno-math-errno -fno-trapping-math -s

TARGET = quiz

all: $(TARGET)

$(TARGET): quiz.c
	$(CC) $(CFLAGS) -fwhole-program $^ -o $(TARGET)
