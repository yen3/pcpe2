VPATH = ../src
CC = clang++
CFLAG = -std=c++11 -stdlib=libc++ -O3 -Wall
DEBUG = -ggdb -D__DEBUG__ -D__GTEST_PCPE__
LIB = -L./gtest -lgtest
INCLUDE = -I../inc -I./gtest/include

OBJ_DIR=build
OBJS=test_main.o hash_table.o esort.o max_comsubseq.o pcpe_util.o

BUILD_OBJS=$(addprefix $(OBJ_DIR)/, $(OBJS))
OUTPUT=test_main.out
	  
all: $(OBJS)
	$(CC) $(DEBUG) $(CFLAG) $(BUILD_OBJS) -o $(OBJ_DIR)/$(OUTPUT) $(LIB) $(INCLUDE)
			 
$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(OBJS): %.o: %.cpp | $(OBJ_DIR)
	$(CC) -c $(DEBUG) $(CFLAG) $< -o $(OBJ_DIR)/$@ $(INCLUDE)

					  
clean:
	-rm -f $(BUILD_OBJS)
	-rm -f $(OBJ_DIR)/$(OUTPUT)
	-rm -rf $(OBJ_DIR)
