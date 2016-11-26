# gcc version >= 4.8
CXX = g++

SRC_DIRS ?= src \
			../src
INC_DIRS   = ../inc \
			 ./gtest/googletest/include
BUILD_DIR ?= ./build

PCPE2_TEST=$(BUILD_DIR)/test_main.out

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CPPFLAGS = $(addprefix -I,$(INC_DIRS))  -MMD -MP
DEBUG_FLAGS = -g3 -O0
CXXFLAGS = -std=c++11 -Wall -Wextra -D__DEBUG__ -D__GTEST_PCPE__ $(DEBUG_FLAGS)
LDFLAGS = -L./gtest/googletest -lgtest -lpthread

$(PCPE2_TEST): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $(PCPE2_TEST) $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

.PHONY: test
test:
	make -j4 && ./build/test_main.out


.PHONY: clean
clean:
	rm -rf $(OBJS)
	rm -rf $(PCPE2_TEST)

-include $(DEPS)
