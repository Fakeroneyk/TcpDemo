TARGET_DIR=./Libs/

CC	= gcc
CXX	= g++
AR	= ar

ARFLAGS = cr
CPPFLAGS= -g -Wall $(INC) -fPIC -std=c++11

TARGET	= $(TARGET_DIR)/libcommon.a
OBJ_DIR	= /.objs

INC	= -I$(COMMON_INCLUDE_DIR) -I$(INCLUDE_DIR)
	VPATH	= .:$(SERVER_DIR)

SRC = $(wildcard *.cpp)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .cpp,.o,$(SRC)))
all : $(TARGET)

$(TARGET) : $(OBJS)
			  @if [ ! -e $(TARGET_DIR)  ]; then \
				  mkdir -p $(TARGET_DIR); \
				  fi
				  $(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : %.cpp
				  @if [ ! -e $(OBJ_DIR)  ]; then \
					  mkdir -p $(OBJ_DIR); \
					  fi
					  $(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)
