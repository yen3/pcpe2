VPATH = src
CXX = g++
CXXFLAG = -std=c++11 -O2 -Wall -Wextra
DEBUG = -g3 -D__DEBUG__
LIB =
INCLUDE = -I./inc

OBJ_DIR=build
OBJS=main.o hash_table.o esort.o max_comsubseq.o pcpe_util.o

BUILD_OBJS=$(addprefix $(OBJ_DIR)/, $(OBJS))
OUTPUT=main.out

all: $(OBJS)
	$(CXX) $(DEBUG) $(CXXFLAG) $(BUILD_OBJS) -o $(OBJ_DIR)/$(OUTPUT) $(LIB) $(INCLUDE) -lpthread

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(OBJS): %.o: %.cpp | $(OBJ_DIR)
	$(CXX) -c $(DEBUG) $(CXXFLAG) $< -o $(OBJ_DIR)/$@ $(INCLUDE)


clean:
	-rm -f $(BUILD_OBJS)
	-rm -f $(OBJ_DIR)/$(OUTPUT)
	-rm -rf $(OBJ_DIR)
