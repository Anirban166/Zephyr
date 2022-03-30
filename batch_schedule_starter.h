#include <cmath>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std::chrono;
using timestamp = seconds;
enum STATUS
{
  RUNNING, WAITING, CANCELLED
};
class Job 
{
    public:
      int jobNum;
      int startTime;
      timestamp startTime;
      timestamp waitTime;
      timestamp requestedRunTime;
      timestamp trueRunTime;
      timestamp stopTime;
      double requestedCPUs;
      //Actual time run on each CPU. CPU 1 == trueRunTime. multiple CPUs (parallel) - take all the time measurements, average it
      std::vector<timestamp> cpuTimes; 
      double requestedMemory;
      double usedMemory;
      STATUS jobStatus;
      int userId;
      int groupId;
      int precedingJobId;
      timestamp thinkTimeFromPreceding;
};

/*
auto startTime = high_resolution_clock::now();
// Code
auto stopTime = high_resolution_clock::now();
// change the duration cast to whatever unit of time you want
auto duration = duration_cast<microseconds>(stopTime - startTime);
*/