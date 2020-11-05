TARGET_DIR = $(CURDIR)/../Bin
TARGET = $(TARGET_DIR)/client

###############################################################################
# variables
###############################################################################
CC  =   gcc
CXX =   g++
CPP =   g++



CPPFLAGS= -fPIC -O2 -Wall -std=c++11 -Wno-deprecated -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function
LDFLAGS = -g -lcommon -lproto_out -lprotobuf

###############################################################################
# paths
###############################################################################
OBJ_DIR = ./.objs

COMMON_INCLUDE_DIR = ../Common
PUBLIC_INC  = -I$(COMMON_INCLUDE_DIR)
LIB_DIR = ../Common/Libs
PUBLIC_LIB  = -L/usr/local/lib/ -L$(LIB_DIR)
SRC = $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.cxx) $(wildcard */*.cpp) $(wildcard */*/*.cpp) $(wildcard */*/*/*.cpp) $(wildcard */*/*/*/*.cpp) $(wildcard */*/*/*/*/*.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .cpp,.o,$(SRC)))

###############################################################################
# args
###############################################################################
all : $(TARGET)

$(TARGET) : $(OBJS)
	@if [ ! -e $(TARGET_DIR) ]; then \
		mkdir -p $(TARGET_DIR); \
	fi

	$(CXX) -o $@ $(OBJS) $(PUBLIC_LIB) $(LDFLAGS)

$(OBJ_DIR)/%.o:%.cpp
	@if [ ! -e $(dir $@) ]; then \
	    mkdir -p $(dir $@); \
	fi
	$(CXX) $(CPPFLAGS) $(PUBLIC_INC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

