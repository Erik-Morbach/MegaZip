CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = 

TARGET = program

SRCS = main.cpp compressor.cpp decompressor.cpp monitor.cpp

OBJS = $(SRCS:.cpp=.o)

HEADERS = compressor.h decompressor.h utils.h monitor.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)