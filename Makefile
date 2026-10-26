
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -pthread

TARGET = banking_system

SOURCES = main.c account.c transaction.c sync.c secalloc.c security.c logger.c report.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean valgrind cppcheck

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -pthread -c $< -o $@

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

cppcheck:
	cppcheck --enable=warning,style,performance,portability --std=c11 $(SOURCES)

clean:
	rm -f $(OBJECTS) $(TARGET)
