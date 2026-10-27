CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11 -pthread

TARGET = banking_system
SOURCES = main.c account.c transaction.c sync.c secalloc.c security.c logger.c report.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all test cppcheck valgrind coverage timestamp clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) -pthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test:
	$(MAKE) gtest

gtest:
	$(CXX) -std=c++17 -pthread tests/test_banking.cpp $(filter-out main.o,$(OBJECTS)) -lgtest -lgtest_main -o banking_tests
	./banking_tests

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem --std=c11 $(SOURCES) 2>&1 | tee cppcheck_report.txt

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

coverage:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) --coverage" LDFLAGS="--coverage" all

timestamp:
	date '+%Y-%m-%d %H:%M:%S'

clean:
	rm -f $(OBJECTS) $(TARGET) banking_tests *.gcda *.gcno *.gcov cppcheck_report.txt
