# Compiler
CXX := g++

# Executable name
TARGET := chess_app

# Source directory
SRC_DIR := src

# Find all .cpp files recursively
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")

# Convert .cpp -> .o
OBJS := $(SRCS:.cpp=.o)

# Compiler flags
CXXFLAGS := -std=c++17 -Wall -Wextra -I$(SRC_DIR)

# SDL2 flags
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS   := $(shell sdl2-config --libs)

# Libraries
LIBS := $(SDL_LIBS) -lboost_filesystem -lpthread

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBS)

# Compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
