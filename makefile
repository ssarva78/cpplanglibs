CC := g++
INCLUDE := -I include -I /usr/local/include/cppunittest
STD := c++20
CPPFLAGS := -Wall -O0 --std=$(STD) 
TEST_DIR := unittest
LIB_DIR := /usr/local/lib
LNK := -L$(LIB_DIR) -lcppunittest
COVFLAGS := -fprofile-arcs -ftest-coverage
REPORT_DIR := reports
CPPCHECK := $(shell which cppcheck 2>/dev/null)
SYSTYPE := $(shell uname)

.PHONY: $(wildcard $(TEST_DIR)/*.cpp)

all: clean verify

clean:
	@echo clean
	@rm -f $(TEST_DIR)/*.out
	@rm -f *.gcno *.gcda **/*.gcno **/*.gcda *.gcov **/*.gcov

verify: unittests codecheck coverage

unittests: $(wildcard $(TEST_DIR)/*.cpp)

codecheck:
	@echo cppcheck=$(CPPCHECK)
ifeq ($(CPPCHECK),)
	@echo cppcheck not installed. skipping static code analysis...
else
	@echo static code analysis...
	@cppcheck --enable=all --language=c++ --error-exitcode=1 -I ./include --suppress=missingIncludeSystem --suppress=cstyleCast --inline-suppr .
endif

$(TEST_DIR)/test_pointer.cpp:
	@$(eval EXEC := $(basename $@).out)
	@$(CC) $(CPPFLAGS) $(COVFLAGS) $(INCLUDE) -o $(EXEC) $@ $(LNK)
	@LD_LIBRARY_PATH=$(LIB_DIR) $(EXEC)
	@mv $(EXEC)-test_pointer.gcno $(TEST_DIR)/test_pointer.gcno
	@mv $(EXEC)-test_pointer.gcda $(TEST_DIR)/test_pointer.gcda
	@echo

$(TEST_DIR)/test_nullable.cpp:
	@$(eval EXEC := $(basename $@).out)
	@$(CC) $(CPPFLAGS) $(COVFLAGS) $(INCLUDE) -o $(EXEC) $@ $(LNK)
	@LD_LIBRARY_PATH=$(LIB_DIR) $(EXEC)
	@mv $(EXEC)-test_nullable.gcno $(TEST_DIR)/test_nullable.gcno
	@mv $(EXEC)-test_nullable.gcda $(TEST_DIR)/test_nullable.gcda
	@echo

docs: clean
	@echo generate documentation
	@rm -rf docs
	@doxygen

coverage:
	@gcov --relative-only --demangled-names $(TEST_DIR)/*.cpp
	@mkdir -p $(REPORT_DIR)
	@mv *.gcov $(REPORT_DIR)/


