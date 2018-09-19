CC = clang
CXX = clang++
CXXFLAGS ?= -std=c++14 -O2 -g
ex01: ex01.cpp
	$(CXX) $(CXXFLAGS) ex01.cpp
