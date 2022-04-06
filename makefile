CXX = g++
CXXFLAGS = -Wall -g -std=c++17

test.bin: sjf.o fcfs.o sharedUtils.o batchScheduler.o
	$(CXX) $(CXXFLAGS) -o test.bin sjf.o fcfs.o sharedUtils.o batchScheduler.o
sjf.o: sjf.cpp
	$(CXX) $(CXXFLAGS) -c sjf.cpp
fcfs.o: fcfs.cpp
	$(CXX) $(CXXFLAGS) -c fcfs.cpp
sharedUtils.o.o: sharedUtils.o
	$(CXX) $(CXXFLAGS) -c sharedUtils.o
batchScheduler.o: batchScheduler.cpp
	$(CXX) $(CXXFLAGS) -c batchScheduler.cpp
clean:	
	rm test.bin sjf.o fcfs.o sharedUtils.o batchScheduler.o