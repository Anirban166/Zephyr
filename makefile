CXX = g++
CXXFLAGS = -Wall -g -std=c++17

test: sjf.o fcfs.o batchScheduler.o
	$(CXX) $(CXXFLAGS) -o test sjf.o fcfs.o batchScheduler.o
sjf.o: sjf.cpp
	$(CXX) $(CXXFLAGS) -c sjf.cpp
fcfs.o: fcfs.cpp
	$(CXX) $(CXXFLAGS) -c fcfs.cpp
batchScheduler.o: batchScheduler.cpp
	$(CXX) $(CXXFLAGS) -c batchScheduler.cpp
clean:	
	rm test sjf.o fcfs.o batchScheduler.o