CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

INCLUDE = include
SRC = src
ALLOC = src/allocator
CACHE = src/cache
BUDDY = src/buddy

SRCS = \
	$(SRC)/main.cpp \
	$(ALLOC)/memory_manager.cpp \
	$(CACHE)/cache.cpp \
	$(BUDDY)/buddy.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = memsim

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run