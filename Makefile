CC = clang
CXX = clang++
CXXFLAGS ?= -std=c++14 -O2 -g

ex1: ex01/ex01.cpp
	$(CXX) $(CXXFLAGS) -o ex1 ex01/ex01.cpp

ex3: ex03/main.cpp
	$(CXX) $(CXXFLAGS) -o ex3 ex03/main.cpp

ex5_1:
	$(CXX) $(CXXFLAGS) -o ex5 ex05/variant1/bfs_std_function.cpp ex05/variant1/bfs_visitor.cpp ex05/variant1/main.cpp

ex5_2:
	$(CXX) $(CXXFLAGS) -o ex5 ex05/variant2/foreach.cpp ex05/variant2/main.cpp

ex7:
	$(CXX) $(CXXFLAGS) -o ex7 ex07/main.cpp

hw1: hw01/main.cpp
	$(CXX) $(CXXFLAGS) -o hw1 hw01/main.cpp
	
hw2: hw02/main.cpp
	$(CXX) $(CXXFLAGS) -o hw2 hw02/main.cpp

hw3: hw03/main.cpp
	$(CXX) $(CXXFLAGS) -o hw3 hw03/main.cpp

hw4: hw04/main.cpp
	$(CXX) $(CXXFLAGS) -o hw4 hw04/main.cpp

c1: c01/main.cpp
	$(CXX) $(CXXFLAGS) -o c1 c01/main.cpp
