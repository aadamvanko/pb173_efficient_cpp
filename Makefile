CC = clang
CXX = clang++
CXXFLAGS ?= -std=c++14 -O2 -g

ex1: ex01/ex01.cpp
	$(CXX) $(CXXFLAGS) -o ex1 ex01/ex01.cpp

ex3: ex03/main.cpp
	$(CXX) $(CXXFLAGS) -o ex3 ex03/main.cpp

hw1: hw01/main.cpp
	$(CXX) $(CXXFLAGS) -o hw1 hw01/main.cpp
