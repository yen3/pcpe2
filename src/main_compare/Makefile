# gcc version >= 4.8
CXX = g++

SRC_DIR   ?= ./src
INC_DIRS   = ./inc
BUILD_DIR ?= ./build

PCPE2=$(BUILD_DIR)/main.out

SRCS := $(SRC_DIR)/esort.cpp \
		$(SRC_DIR)/hash_table.cpp \
		$(SRC_DIR)/main.cpp \
		$(SRC_DIR)/max_comsubseq.cpp \
		$(SRC_DIR)/pcpe_util.cpp

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


CPPFLAGS = $(addprefix -I,$(INC_DIRS))  -MMD -MP
DEBUG_FLAGS = -g3 -D__DEBUG__ -O0
CXXFLAGS = -std=c++11 -Wall -Wextra $(DEBUG_FLAGS)
LDFLAGS  = -lpthread


$(PCPE2): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $(PCPE2) $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: release
release:
	make clean
	make -j4 CXXFLAGS="-std=c++11 -Wall -Wextra -O2 -flto"

.PHONY: clean
clean:
	rm -rf $(OBJS)
	rm -rf $(PCPE2)

-include $(DEPS)
