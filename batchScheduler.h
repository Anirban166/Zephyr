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
enum STATUS { RUNNING, WAITING, CANCELLED };
// --------------
// 1.0: Classes
// --------------
// 1.1) Job Class
class Job 
{
    public:
      int jobNum;                     // Which node is it running on currently
      int nodeId;  
      timestamp submitTime;           // Time the job was submitted by the user 
      timestamp startTime;            // Time the job actually starts running on CPU(s) 
      timestamp waitTime;             // The startTime - submitTime
      timestamp requestedRunTime;     // Inputted by the user on job creation
      timestamp trueRunTime;          // Generated statically when creating job to simulate the amount of time actually needed
      timestamp stopTime;             // startTime + trueRunTime
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
      // Constructor:
      Job(int jobNum,
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
// 1.2) Node Class
class Node 
{
  public:
    int nodeId;                                 // Node identifier
    int coreCount;                             // Total cores in the node
    int memoryAmount;                         // Total memory in the node (in GiB)
    int coresAllocated, memoryAllocated = 0; // Cores and memory used by a job (constrained by above parameters)
    // Constructor:
    Node(int nodeId, int coreCount, int memoryAmount)
    {
      this->nodeId = nodeId;
      this->coreCount = coreCount;
      this->memoryAmount = memoryAmount;
    }
};

// ----------------------
// 2.0: Utility Functions
// ----------------------
// 2.1) 
std::vector<Node> buildNodes(int nodeCount);
// 2.2)
std::vector<Job> buildPresetJobs(std::time_t startTime);
// 2.3)
int isJobValid(Job waitingJob, std::vector<Node> nodeList);
// 2.4) Returns node ID for whichever node has the required resources for the job that requests it, 
// otherwise if all nodes can't satisfy the resource requirements, it returns a -1:
int checkNodeResources(Job waitingJob, std::vector<Node> nodeList);
// 2.5)
bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs);

// --------------------------
// 3.0: Scheduling Algorithms
// --------------------------
// 3.1) Shortest Job First (SJF)
void runSJF(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);
// 3.2) First Come First Serve (FCFS)
void runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);



  
  