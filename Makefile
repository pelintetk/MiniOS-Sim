
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pthread
TARGET = minios

SRC = src/main.c \
      src/sched/parser.c \
      src/sched/scheduler.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
