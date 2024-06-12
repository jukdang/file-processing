main: test.cpp assignment1.h assignment1.cpp
	g++ -std=c++11 test.cpp assignment1.cpp -o main
	make test
	


test: main.exe
	rm student.dat
	./main.exe > result.txt
	