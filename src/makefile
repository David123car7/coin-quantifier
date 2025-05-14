# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -O2 -Iinclude $(shell pkg-config --cflags opencv4)
LDFLAGS = $(shell pkg-config --libs opencv4)

# Source files
SRCS = Source.cpp vc.c
OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

# Output executable
TARGET = coin-quantifier

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cleanup
clean:
	rm -f *.o $(TARGET)
