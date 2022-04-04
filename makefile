CXX = g++
CXXFLAGS = -Wall -g -std=c++17

test: sjf.o fcfs.o batch_schedule_starter.o
	$(CXX) $(CXXFLAGS) -o test sjf.o fcfs.o batch_schedule_starter.o
sjf.o: sjf.cpp
	$(CXX) $(CXXFLAGS) -c sjf.cpp
fcfs.o: fcfs.cpp
	$(CXX) $(CXXFLAGS) -c fcfs.cpp
batch_schedule_starter.o: batch_schedule_starter.cpp
	$(CXX) $(CXXFLAGS) -c batch_schedule_starter.cpp
clean:	
	rm test sjf.o fcfs.o batch_schedule_starter.o
