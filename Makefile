CC = clang
CXX = clang++
CXXFLAGS ?= -std=c++14 -O2 -g

ex1: ex01/ex01.cpp
	$(CXX) $(CXXFLAGS) -o ex1 ex01/ex01.cpp

hw1: hw01/main.cpp
	$(CXX) $(CXXFLAGS) -o hw1 hw01/main.cpp
