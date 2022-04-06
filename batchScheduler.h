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
enum STATUS { RUNNING, WAITING, QUEUED, CANCELLED };
// --------------
// 1.0: Classes
// --------------
// 1.1) Job Class
class Job 
{
    public:
      int jobNum;                           // Unique job identifier
      int nodeId;                           // Node at which the job is running (multiple jobs can have one nodeId)
      timestamp submitTime;                 // Time the job was submitted by the user 
      timestamp startTime;                  // Time the job actually starts running on CPU(s) 
      timestamp waitTime;                   // Difference between startTime and submitTime
      timestamp requestedRunTime;           // User requested (on job creation) job run time
      timestamp trueRunTime;                // Generated statically when creating job to simulate the amount of time actually needed
      timestamp stopTime;                   // Sum of startTime and trueRunTime
      STATUS jobStatus = WAITING;           // Current state of the job (default is wait mode)
      int userId, groupId, precedingJobId;  // Identifiers for additional filtering
      double requestedCPUs, usedCPUs;       // Requested and actually used CPU cores
      double requestedMemory, usedMemory;   // Requested and actually used memory (GiB)
      // Actual runtime on each CPU: (1 CPU == trueRunTime, multiple CPUs or parallel core usage => take all the time measurements, get the average)
      std::vector<timestamp> cpuTimes;

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
    int nodeId;                   // Node identifier
    int coreCount;               // Total cores in the node
    int memoryAmount;           // Total memory in the node (in GiB)
    int coresAllocated = 0;    // Cores used by a job (constrained by above parameters)
    int memoryAllocated = 0;  // Memory used by a job (constrained by above parameters)
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
// 2.1) Function to build nodes and return a vector of all the available nodes:
std::vector<Node> buildNodes(int nodeCount);
// 2.2) Function to build preset jobs and return a vector of all the jobs:
std::vector<Job> buildPresetJobs(std::time_t startTime);
// 2.2.1) Either replace the above with randomized jobs or use this instead after initial testing:
// std::vector<Job> buildRandomizedJobs(int jobCount, std::time_t startTime)
// 2.3) Checks for job validity, returns node ID for whichever node first has the requested resources under its maximum bounds,
// otherwise -1 if the request is above the limits for all the nodes:
int isJobValid(Job waitingJob, std::vector<Node> nodeList);
// 2.4) Returns node ID for whichever node has the required resources for the job that requests it, 
// otherwise if all nodes can't satisfy the resource requirements, it returns a -1:
int checkNodeResources(Job waitingJob, std::vector<Node> nodeList);

std::vector<Job> verifyJobs(std::vector<Job> jobList, std::vector<Node> nodeList);
// 2.5) Function to indicate the end of simulation, when the joblist and queues are empty at the very end:
bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs);
// 2.6) Function to print jobs:
void printJobs(std::vector<Job> jobs);

// Notes:
// 1) Keep definitions of utility functions in the required files
// In case of two or more files requiring the same function, 
// just emplace one function once to avoid linker errors.
// 2) Making isJobValid() return a boolean will create an infinite loop

// --------------------------
// 3.0: Scheduling Algorithms
// --------------------------
// 3.1) Shortest Job First (SJF)
void runSJF(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);
// 3.2) First Come First Serve (FCFS)
void runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);