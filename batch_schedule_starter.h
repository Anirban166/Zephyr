#include <ctime>
#include <cmath>
#include <random>
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
      //Time the job was submitted by the user
      timestamp submitTime;
      //Time the job actually starts running on CPU(s) 
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

      Job(
        int jobNum,
        timestamp startTime,
        timestamp waitTime,
        timestamp requestedRunTime,
        timestamp trueRunTime,
        timestamp stopTime,
        double requestedCPUs,
        std::vector<timestamp> cpuTimes,
        double requestedMemory,
        double usedMemory,
        STATUS jobStatus,
        int userId,
        int groupId,
        int precedingJobId,
        timestamp thinkTimeFromPreceding){
          this->jobNum = jobNum;
          this->startTime = startTime;
          this->waitTime = waitTime;
          this->requestedRunTime = requestedRunTime;
          this->trueRunTime = trueRunTime;
          this->stopTime = stopTime;
          this->requestedCPUs = requestedCPUs;
          //Actual time run on each CPU. CPU 1 == trueRunTime. multiple CPUs (parallel) - take all the time measurements, average it
          this->cpuTimes = cpuTimes; 
          this->requestedMemory = requestedMemory;
          this->usedMemory = usedMemory;
          this->jobStatus = jobStatus;
          this->userId = userId;
          this->groupId = groupId;
          this->precedingJobId = precedingJobId;
          this->thinkTimeFromPreceding = thinkTimeFromPreceding;
      }
};


class Node {
  int coreCount;
  int coresUsed;
  int memoryAmount;
  int memoryUsed;

  Node(int coreCount, int coresUsed, int memoryAmount, int memoryUsed){
    this->coreCount = coreCount;
    this->coresUsed = coresUsed;
    this->memoryAmount = memoryAmount;
    this->memoryUsed = memoryUsed;
  }

};

std::vector<Job> buildPresetJobs();

/*
auto startTime = high_resolution_clock::now();
// Code
auto stopTime = high_resolution_clock::now();
// change the duration cast to whatever unit of time you want
auto duration = duration_cast<microseconds>(stopTime - startTime);
*/