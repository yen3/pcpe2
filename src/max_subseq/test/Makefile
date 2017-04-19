# gcc version >= 4.8
CXX = g++

SRC_DIR   ?= ./src
INC_DIRS   = ../inc \
			 ./gtest/googletest/include
BUILD_DIR ?= ./build

MAX_SUBSEQ_TEST=$(BUILD_DIR)/test_main

SRCS := $(SRC_DIR)/test_main.cc \
	    $(SRC_DIR)/test_logging.cc \
	    $(SRC_DIR)/../../src/seq.cc \
	    $(SRC_DIR)/../../src/logging.cc
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CPPFLAGS = $(addprefix -I,$(INC_DIRS))  -MMD -MP
DEBUG_FLAGS = -g3 -O0
CXXFLAGS = -std=c++11 -Wall -Wextra $(DEBUG_FLAGS)
LDFLAGS = -L./gtest/googletest -lgtest -lpthread

$(MAX_SUBSEQ_TEST): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $(MAX_SUBSEQ_TEST) $(LDFLAGS)

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: test_and_save_result
test_and_save_result:
	make -j4 && ./${MAX_SUBSEQ_TEST}

.PHONY: test
test:
	make test_and_save_result
	@cd testoutput && rm -rf *

.PHONY: clean
clean:
	rm -rf $(OBJS)
	rm -rf $(DEPS)
	rm -rf $(MAX_SUBSEQ_TEST)

-include $(DEPS)
