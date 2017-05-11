CXXFLAGS ?= -std=c++11 
lmerge: lmerge.cpp
	c++ $(CXXFLAGS) -o lmerge lmerge.cpp
