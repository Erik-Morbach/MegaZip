CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = 

TARGET = main

SRCS = compressor.cpp decompressor.cpp monitor.cpp main.cpp

all: 
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)