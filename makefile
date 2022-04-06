CXX = g++
CXXFLAGS = -Wall -g -std=c++17

test.bin: sjf.o fcfs.o batchScheduler.o
	$(CXX) $(CXXFLAGS) -o test.bin sjf.o fcfs.o batchScheduler.o
sjf.o: sjf.cpp
	$(CXX) $(CXXFLAGS) -c sjf.cpp
fcfs.o: fcfs.cpp
	$(CXX) $(CXXFLAGS) -c fcfs.cpp
batchScheduler.o: batchScheduler.cpp
	$(CXX) $(CXXFLAGS) -c batchScheduler.cpp
clean:	
	rm test.bin sjf.o fcfs.o batchScheduler.o