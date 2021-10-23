TARGET = main

PROJECT_DIR = .

INCLUDES = -I$(PROJECT_DIR)/include 


CC = gcc
CXX = g++
AR = ar  
CC_FLAGS += -g -Wall -std=c++11
  
SRCS = ./preparation.cpp \
	./cubestate.cpp \
	./utils.cpp	\
	./main.cpp 


OBJS=$(SRCS:.c=.o)

.PHONY: all clean

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CXX) $(CC_FLAGS) $(INCLUDES)  -o $(TARGET) $^ 

%.o:%.cpp
	$(CXX) -c $(CC_FLAGS) $(INCLUDES)  $< -o $@ 
   
clean:
	rm -f $(SRCS:.cpp=.o) $(TARGET)