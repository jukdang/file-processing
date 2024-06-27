main: main.cpp bptree.h bptree.cpp
	g++ -std=c++11 main.cpp bptree.cpp -o main
	./main.exe > result.txt
	
test: 
	./main.exe > result.txt