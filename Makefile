

CC = gcc
CXX = g++

CFLAGS = -Wall -Wextra -std=c11 -g
CXXFLAGS = -Wall -Wextra -std=c++17 -g
THREAD_FLAGS = -pthread
CPPFLAGS = -I.

APP = banking_system
BUILD_DIR = build

APP_SOURCES = main.c account.c transaction.c sync.c secalloc.c security.c logger.c report.c
APP_OBJECTS = $(APP_SOURCES:%.c=$(BUILD_DIR)/%.o)

TEST_DIR = tests
TEST_SOURCE = $(TEST_DIR)/test_main.cpp
TEST_OBJECT = $(BUILD_DIR)/test_main.o
TEST_SUPPORT_SOURCES = account.c transaction.c sync.c security.c logger.c secalloc.c
TEST_SUPPORT_OBJECTS = $(TEST_SUPPORT_SOURCES:%.c=$(BUILD_DIR)/test_%.o)
TEST_APP = $(BUILD_DIR)/banking_tests

.PHONY: all test gtest analyze cppcheck valgrind coverage coverage-report timestamp clean help

# Build the banking application
all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_OBJECTS)
	$(CC) $(APP_OBJECTS) -o $@ $(THREAD_FLAGS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(THREAD_FLAGS) -c $< -o $@

# Build Google Test unit tests
$(TEST_OBJECT): $(TEST_SOURCE) | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%.o: %.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(THREAD_FLAGS) -c $< -o $@

$(TEST_APP): $(TEST_OBJECT) $(TEST_SUPPORT_OBJECTS)
	$(CXX) $^ -o $@ -lgtest -lgtest_main $(THREAD_FLAGS)

test: gtest

gtest: $(TEST_APP)
	./$(TEST_APP)

# Static code analysis
analyze: cppcheck

cppcheck:
	cppcheck --enable=warning,style,performance,portability --error-exitcode=1 --std=c11 --suppress=missingIncludeSystem $(APP_SOURCES)

# Memory checking (requires Valgrind)
valgrind: $(APP)
	printf 'admin\nadmin123\n9\n' | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ./$(APP)

# Rebuild with code coverage instrumentation
coverage:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) --coverage" all

# Generate coverage reports after running the instrumented application
coverage-report:
	gcov -b -c $(APP_SOURCES)

# Display the current date and time
timestamp:
	@date '+%Y-%m-%d %H:%M:%S'

# Remove generated files
clean:
	rm -rf $(BUILD_DIR) $(APP) banking_tests
	rm -f *.gcda *.gcno *.gcov cppcheck_report.txt

# Display available commands
help:
	@echo "Available commands:"
	@echo "  make                 Build the application"
	@echo "  make test            Run Google Test unit tests"
	@echo "  make cppcheck        Run static code analysis"
	@echo "  make valgrind        Run memory checks"
	@echo "  make coverage        Rebuild with coverage instrumentation"
	@echo "  make coverage-report Generate gcov coverage reports"
	@echo "  make timestamp       Display current date and time"
	@echo "  make clean           Remove generated files"
	@echo "  make help            Display this help"
