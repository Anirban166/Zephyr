#pragma once
#include <ctime>
#include <cmath>
#include <random>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
using namespace std::chrono;
using timestamp = std::time_t;
enum STATUS
{
  RUNNING, WAITING, CANCELLED
};
class Job 
{
    public:
      int jobNum;
      //Which node is it running on currently
      int nodeId;
      //Time the job was submitted by the user
      timestamp submitTime;
      //Time the job actually starts running on CPU(s) 
      timestamp startTime;
      //The startTime - submitTime
      timestamp waitTime;
      //Inputted by the user on job creation
      timestamp requestedRunTime;
      //Generated statically when creating job to simulate the amount of time actually needed
      timestamp trueRunTime;
      //startTime + trueRunTime
      timestamp stopTime;
      double requestedCPUs;
      double usedCPUs;
      //Actual time run on each CPU. CPU 1 == trueRunTime. multiple CPUs (parallel) - take all the time measurements, average it
      std::vector<timestamp> cpuTimes; 
      double requestedMemory;
      double usedMemory;
      STATUS jobStatus = WAITING;
      int userId;
      int groupId;
      int precedingJobId;

      Job(
        int jobNum,
        std::time_t submitTime,
        std::time_t requestedRunTime,
        std::time_t trueRunTime,
        double requestedCPUs,
        double usedCPUs,
        double requestedMemory,
        double usedMemory)
        {
          this->jobNum = jobNum;
          this->submitTime = submitTime;
          this->requestedRunTime = requestedRunTime;
          this->trueRunTime = trueRunTime;
          this->requestedCPUs = requestedCPUs;
          this->usedCPUs = usedCPUs;
          this->requestedMemory = requestedMemory;
          this->usedMemory = usedMemory;
      }
};

class Node 
{
  public:
    int nodeId;
    //Total cores in the node
    int coreCount;
    //Cores in use by a job
    int coresAllocated = 0;
    //Total cores in the node in Gib
    int memoryAmount;
    //Memory in use by a job
    int memoryAllocated = 0;
    Node(int nodeId, int coreCount, int memoryAmount)
    {
      this->nodeId = nodeId;
      this->coreCount = coreCount;
      this->memoryAmount = memoryAmount;
    }
};

std::vector<Job> buildPresetJobs(std::time_t startTime);
std::vector<Node> buildNodes(int nodeCount);
bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs);
//First Come First Serve (FCFS)
void runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);

//Shortest Job First (SJF)
void runSJF(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);
int checkNodeResources(Job waitingJob, std::vector<Node> nodeList);
int isJobValid(Job waitingJob, std::vector<Node> nodeList);



  
  