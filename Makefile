TARGET=mymd5sum
CXXFLAGS=-g -Wall -Og
CFLAGS=-g -Wall -Og
LDFLAGS= 
CXX=g++
CC=gcc
INCLUDES=
BIN_DIR=bin

CPP_SOURCES = $(wildcard *.cpp)
C_SOURCES = $(wildcard *.c)

CPP_OBJS += $(patsubst %.cpp, %.o, $(CPP_SOURCES))
C_OBJS += $(patsubst %.c, %.o, $(C_SOURCES))

OBJS_DEP = $(patsubst %.cpp, %.o.d, $(CPP_SOURCES)) $(patsubst %.c, %.o.d, $(C_SOURCES))

OBJS = $(CPP_OBJS) $(C_OBJS) 

# $(info $(OBJS_DEP))
-include $(OBJS_DEP)

.PHONY: all clean

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS)  -o  $*.o -c $< -MD -MF "$(dir $@)$(notdir $@).d"

.cpp.o:
# @echo $* $< $@ $(dir $@) $(notdir $@).d
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o  $*.o -c $< -MD -MF "$(dir $@)$(notdir $@).d"

all:$(OBJS) $(CPP_SOURCES) $(C_SOURCES)
	@echo $(OBJS)
	@if [ ! -d $(BIN_DIR) ]; then mkdir -p $(BIN_DIR); fi
	$(CXX) $(LDFLAGS) $(OBJS) -o $(BIN_DIR)/$(TARGET)

clean:
	@rm -rf $(BIN_DIR)/ $(OBJS) $(OBJS_DEP)

