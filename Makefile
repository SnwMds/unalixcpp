CXX ?= c++
CXXFLAGS ?= -s -Os -flto=full -w -Wfatal-errors -ljsoncpp -std=c++20
TARGET = main
OBJECT = src/uri.o src/utils.o src/clear_url.o src/main.o
SRC = src/uri.cpp src/utils.cpp src/clear_url.cpp src/main.cpp

all : $(OBJECT)
	$(CXX) $(CXXFLAGS) $(OBJECT) -o /data/data/com.termux/files/home/$(TARGET)

utils.o : utils.hpp
uri.o : uri.hpp
clear_url.o : clear_url.hpp

main.o : clear_url.cpp utils.hpp uri.hpp

.PHONY:	clean

clean:
	rm $(OBJECT) $(TARGET)