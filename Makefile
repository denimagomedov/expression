CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -Iinclude
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O3

SRC_DIR := src
INC_DIR := include
LIB_SRCS := $(SRC_DIR)/expression.cpp
MAIN_SRC := $(SRC_DIR)/eval.cpp
DEPS := $(wildcard $(INC_DIR)/*.hpp)

LIB_OUT := libexpression.a
TARGET := expression_test

BUILD_MODE ?= debug

ifeq ($(BUILD_MODE),debug)
    CXXFLAGS += $(DEBUG_FLAGS)
else
    CXXFLAGS += $(RELEASE_FLAGS)
endif

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(LIB_OUT)
	$(CXX) $(CXXFLAGS) $< -L. -lexpression -o $@

$(LIB_OUT): $(LIB_SRCS) $(DEPS)
	$(CXX) $(CXXFLAGS) -c $(LIB_SRCS) -o expression.o
	ar rcs $@ expression.o

clean:
	rm -f *.o $(TARGET) $(LIB_OUT)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
