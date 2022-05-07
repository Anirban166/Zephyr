#pragma once
#include <ctime>
#include <cmath>
#include <random>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std::chrono;
using timestamp = std::time_t;
const int NO_JOB_RESERVING = -1;
const int NO_SHADOW_TIME_ASSIGNED = -99;
enum STATUS
{
  RUNNING,
  WAITING,
  QUEUED,
  CANCELLED
};
template <typename... T>
void print(T &&...args)
{
  ((std::cout << args), ...);
}
// --------------
// 1.0: Classes
// --------------
// 1.1) Job Class
class Job
{
public:
  int jobNum;                          // Unique job identifier
  int nodeID;                          // Node at which the job is running (multiple jobs can have one nodeId)
  double stretch;                      // Slowdown (wait time + run time)/runtime
  timestamp waitTime;                  // Difference between startTime and submitTime                           //
  timestamp stopTime;                  // Sum of startTime and trueRunTime
  timestamp startTime;                 // Time the job actually starts running on CPU(s)
  timestamp submitTime;                // Time the job was submitted by the user
  timestamp trueRunTime;               // Generated statically when creating job to simulate the amount of time actually needed
  timestamp turnAroundTime;            // Difference between the times of job submission and completion
  timestamp requestedRunTime;          // User requested (on job creation) job run time
  timestamp shadowTime;                // Time that this job is guaranteed to start running
  STATUS jobStatus = WAITING;          // Current state of the job (default is wait mode)
  double requestedCPUs, usedCPUs;      // Requested and actually used CPU cores
  double requestedMemory, usedMemory;  // Requested and actually used memory (GiB)
  int userId, groupId, precedingJobId; // Identifiers for additional filtering
  // std::vector<timestamp> cpuTimes;

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
    this->usedCPUs = usedCPUs;
    this->usedMemory = usedMemory;
    this->submitTime = submitTime;
    this->trueRunTime = trueRunTime;
    this->requestedCPUs = requestedCPUs;
    this->requestedMemory = requestedMemory;
    this->requestedRunTime = requestedRunTime;
  }
};
// 1.2) Node Class
class Node
{
public:
  int nodeID;              // Node identifier
  int coreCount;           // Total cores in the node
  int memoryAmount;        // Total memory in the node (in GiB)
  int coresAllocated = 0;  // Cores used by a job (constrained by above parameters)
  int memoryAllocated = 0; // Memory used by a job (constrained by above parameters)
  // Constructor:
  Node(int nodeID, int coreCount, int memoryAmount)
  {
    this->nodeID = nodeID;
    this->coreCount = coreCount;
    this->memoryAmount = memoryAmount;
  }
};
// 1.2) Metrics Class
class Metrics
{
public:
  std::string algorithm;
  timestamp totalWaitSum = 0;
  timestamp averageWait = 0;
  timestamp longestWait = 0;
  timestamp avgturnAroundTime = 0;
  timestamp totalturnAroundTime = 0;
  timestamp maxTurnAroundTime = 0;
  double avgStretch = 0;
  double totalStretch = 0;
  double maxStretch = 0;
  int totalCPUsUsed = 0;
  // int totalCPUsReqd = 0;
  int maxCPUsUsed = 0;
  unsigned long totalMemoryUsed = 0;
  unsigned long maxMemoryUsed = 0;
  int averageCPUsUsed = 0;
  unsigned long averageMemoryUsed = 0;
  int totalJobsRun = 0;
  // Constructor:
  Metrics(std::string algorithm)
  {
    this->algorithm = algorithm;
  }
};

// ----------------------
// 2.0: Utility Functions
// ----------------------
// 2.1) Functions to build stuff:
// 2.1.1) Build nodes and return a vector of all the available nodes:
std::vector<Node> buildNodes(int nodeCount);
// 2.1.2) Build preset jobs and return a vector of all the jobs:
std::vector<Job> buildPresetJobs(std::time_t startTime, std::string algorithm);
// 2.1.3) Build randomized parameters for a specified (input) number of jobs:
std::vector<Job> buildRandomizedJobs(int jobCount, std::time_t startTime);
// 2.2) Checks for job validity, returns node ID for whichever node first has the requested resources under its maximum bounds,
// otherwise -1 if the request is above the limits for all the nodes:
int isJobValid(Job waitingJob, std::vector<Node> nodeList);
// 2.3) Returns node ID for whichever node has the required resources for the job that requests it,
// otherwise if all nodes can't satisfy the resource requirements, it returns a -1:
int checkNodeResources(Job waitingJob, std::vector<Node> nodeList);
// 2.4) Function to verify the feasibility of jobs:
std::vector<Job> verifyJobs(std::vector<Job> jobList, std::vector<Node> nodeList);
// 2.5) Function to indicate the end of simulation, when the joblist and queues are empty at the very end:
bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs);
// 2.6) Functions to print stuff:
// 2.6.1) Function to print jobs:
void printJobs(std::vector<Job> jobs);
// void printJobsToFile(std::vector<Job> jobs, std::ofstream file);
// 2.6.2) Function to print reserved jobs:
void printReservedJobs(std::vector<Job> jobs);
// 2.6.3) Function to print the finalized (post operations such as taking the average) metrics that we are concerned with:
void finalizeAndOutputMetrics(Metrics metrics, std::string fileName);
// 2.7) Conditionals:
// 2.7.1) Function to indicate if a job can finish before the time at which the first job in the queue finishes execution:
bool canFinishBeforeShadow(timestamp shadowTime, timestamp reqRuntime, timestamp currentTime);
// 2.7.2) Special case of the above for conservative backfilling:
bool canFinishBeforeShadowCBF(std::vector<Job> runningJobs, timestamp reqRuntime, int targetNodeId, timestamp currentTime);
// 2.7.3) Function to reserve jobs:
bool jobsReserving(std::vector<Job> jobQueue);
// 2.8) Metrics or timestamp returning functions:
// 2.8.1) Function to run the input scheduling algorithm and return the metrics for the simulated run of the selected algorithm:
Metrics runAlgorithm(std::string selectedAlgorithm);
// 2.8.2) Function to return the shadow time from the preeceding jobs:
timestamp findShadowTimeFromPreceedingJobs(std::vector<Job> runningJobs, int targetNodeId);
// 2.9) Miscellaneous:
// 2.9.1) Function to update the shadow time of the next waiting job:
void updateShadowTimeOfNext(std::vector<Job> reservingJobs, Job selectedJob, int targetNodeId);
// 2.9.2) Function to generate random numbers (from the uniform distribution) in a range:
double rangeRNG(double lowerLimit, double upperLimit);

// Iterator notes:
// We are currently iterating on the reverse since erase() moves everything to the left to fill the hole.
// Alternatives:
// (a) Increment prior to erasing:
// i.e., remove the increment/decrement from the loop and use jobList.erase(++currentJobIter);
// (b) Using a loop index:
// const std::size_t size = v.size();
// for(std::size_t i = 0; i < size; ++i) v.erase(v[i]);
// The ith element will remain the ith element this way (unlike the problem case where the iterator points to the next element, skipping past one)

// --------------------------
// 3.0: Scheduling Algorithms
// --------------------------
// 3.1) Shortest Job First (SJF)
Metrics runSJF(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);
// 3.2) First Come First Serve (FCFS)
Metrics runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);
// 3.3) Extensible Argonne Scheduling System (EASY)
Metrics runEASY(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);
// 3.4) Extensible Argonne Scheduling System (CBF)
Metrics runCBF(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime);