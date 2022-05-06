C = g++
M_DIR = ./Source/
ALGOS = ./Source/Algorithms/
CFLAGS = -Wall -g -std=c++17 -Iinclude

test.bin: SJF.o FCFS.o EASY.o CBF.o sharedUtils.o batchScheduler.o
	$(C) $(CFLAGS) -o test.bin SJF.o FCFS.o EASY.o CBF.o sharedUtils.o batchScheduler.o

SJF.o: $(ALGOS)SJF.cpp
	$(C) $(CFLAGS) -c $(ALGOS)SJF.cpp

FCFS.o: $(ALGOS)FCFS.cpp
	$(C) $(CFLAGS) -c $(ALGOS)FCFS.cpp

EASY.o: $(ALGOS)EASY.cpp
	$(C) $(CFLAGS) -c $(ALGOS)EASY.cpp

CBF.o: $(ALGOS)CBF.cpp
	$(C) $(CFLAGS) -c $(ALGOS)CBF.cpp

sharedUtils.o: $(M_DIR)sharedUtils.cpp
	$(C) $(CFLAGS) -c $(M_DIR)sharedUtils.cpp

batchScheduler.o: $(M_DIR)batchScheduler.cpp
	$(C) $(CFLAGS) -c $(M_DIR)batchScheduler.cpp

clean:	
	rm test.bin SJF.o FCFS.o EASY.o CBF.o sharedUtils.o batchScheduler.o