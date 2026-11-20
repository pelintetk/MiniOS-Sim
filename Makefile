
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pthread
TARGET = minios
SRC = src/main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
