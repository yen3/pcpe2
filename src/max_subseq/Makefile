# gcc version >= 4.8
CXX = g++

SRC_DIR   ?= ./src
INC_DIRS   = ./inc
BUILD_DIR ?= ./build

MAX_SUBSEQ=$(BUILD_DIR)/max_subseq.out

SRCS := $(SRC_DIR)/seq.cc \
        $(SRC_DIR)/com_subseq.cc \
        $(SRC_DIR)/small_seq_hash.cc \
        $(SRC_DIR)/pcpe_util.cc \
        $(SRC_DIR)/logging.cc \
    		$(SRC_DIR)/main.cc

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)


CPPFLAGS = $(addprefix -I,$(INC_DIRS))  -MMD -MP
DEBUG_FLAGS = -g3 -D__DEBUG__ -O0
CXXFLAGS = -std=c++11 -Wall -Wextra $(DEBUG_FLAGS)
LDFLAGS  = -lpthread


$(MAX_SUBSEQ): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $(MAX_SUBSEQ) $(LDFLAGS)

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: release
release:
	make clean
	make -j4 CXXFLAGS="-std=c++11 -Wall -Wextra -O2"

.PHONY: clean
clean:
	rm -rf $(OBJS)
	rm -rf $(MAX_SUBSEQ)

-include $(DEPS)
