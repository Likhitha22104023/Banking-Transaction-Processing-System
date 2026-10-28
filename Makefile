CC = gcc
CXX = g++

CFLAGS = -g -Wall -Wextra -std=c11 -pthread
CXXFLAGS = -g -Wall -Wextra -std=c++17 -pthread
LDFLAGS = -pthread --coverage

TARGET = banking_system
SOURCES = main.c account.c transaction.c sync.c secalloc.c security.c logger.c report.c
OBJECTS = $(SOURCES:.c=.o)

TEST_SRC = test_main.cpp
TEST_TARGET = banking_tests
TEST_OBJECTS = $(filter-out main.o,$(OBJECTS))

.PHONY: all test gtest coverage coverage-report cppcheck valgrind run clean timestamp audit help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

gtest: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) $(TEST_OBJECTS) $(LDFLAGS) -lgtest -lgtest_main -o $(TEST_TARGET)

test: gtest

coverage:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) --coverage" CXXFLAGS="$(CXXFLAGS) --coverage" LDFLAGS="$(LDFLAGS) --coverage" test

coverage-report:
	gcov $(SOURCES)

cppcheck:
	cppcheck --enable=warning,performance,portability --std=c11 --suppress=missingIncludeSystem $(SOURCES)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

run: $(TARGET)
	./$(TARGET)

timestamp:
	date '+%Y-%m-%d %H:%M:%S'

clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET) *.gcda *.gcno *.gcov cppcheck_report.txt testing_audit.log

audit:
	@echo "Timestamp: $$(date)" > testing_audit.log
	@echo "Building application..." >> testing_audit.log
	-$(MAKE) clean >> testing_audit.log 2>&1
	-$(MAKE) all >> testing_audit.log 2>&1
	@echo "Running tests..." >> testing_audit.log
	-$(MAKE) test >> testing_audit.log 2>&1
	@echo "Running Cppcheck..." >> testing_audit.log
	-$(MAKE) cppcheck >> testing_audit.log 2>&1
	@echo "Audit completed. Check testing_audit.log"

help:
	@echo "Available targets:"
	@echo "  make              Build the banking application"
	@echo "  make test         Compile and run Google Test cases"
	@echo "  make coverage     Run tests with coverage instrumentation"
	@echo "  make coverage-report Generate gcov reports"
	@echo "  make cppcheck     Run static analysis"
	@echo "  make valgrind     Check memory usage"
	@echo "  make run          Run the application"
	@echo "  make timestamp    Display date and time"
	@echo "  make audit        Run build, tests and static analysis"
	@echo "  make clean        Remove generated files"
