#include "batch_schedule_starter.h"
// Compile and execute using:
// g++ batch_schedule_starter.cpp -o batch_schedule_starter && ./batch_schedule_starter 
/**
   Args: Number of jobs, CPUs available, Memory Available, Nodes available,
   Scheduling algorithm, Verbose
 **/ 
int main(int argc, char* argv[]) 
{
    if(argc != 3)
    {
        std::cout << "Please enter the following arguments:\n";  
        std::cout << "The number of jobs to schedule.\n";
        std::cout << "The scheduling algorithm to use.\n"; 
        exit(-1); 
    }    
    std::cout << "The arguments you entered are:\n";
    for(int i = 0; i < argc; ++i) 
        std::cout << "Argument " << i << ": " << argv[i] << "\n";  

    int jobCount = atoi(argv[1]);
    std::string schedulingAlgorithm = argv[2];
    int nodeCount = 3;
    int maxCoresPerNode = 24;
    //100 GiB per node, i.e. 102400 MiB
    int maxMemoryPerNode = 102400;
    
    //Get the current time to reference as a start point  for the entire simulation
    time_point<system_clock, seconds> startTimePoint = time_point_cast<seconds>(system_clock::now());
    std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);

    //Generate nodes
    std::vector<Node> nodeList = buildNodes(nodeCount);
    // Generate jobs
    std::vector<Job> jobList = buildPresetJobs(startTime);
    
    // Choose algorithm to run
    if(!schedulingAlgorithm.compare("FCFS")) 
    { 
         //runFCFS
    }
    else if(!schedulingAlgorithm.compare("SJF"))
    {
        runSJF(nodeList, jobList, startTime);
    }
}

std::vector<Node> buildNodes(int nodeCount)
{
    std::vector<Node> nodeList;
    // coreCount, coresAllocated, memoryAmount, memoryAllocated
    Node firstNode = Node(0, 24, 102400);
    Node secondNode = Node(1, 24, 102400);
    Node thirdNode = Node(2, 24, 102400);
    nodeList.push_back(firstNode);
    nodeList.push_back(secondNode);
    nodeList.push_back(thirdNode);
    return nodeList;
}

std::vector<Job> buildPresetJobs(std::time_t startTime)
{   
  std::vector<Job> jobList;
  // jobNum, submitTime, requestedRunTime, trueRunTime, requestedCPUs, usedCPUs, requestedMem, usedMem
  Job firstJob = Job(1, startTime + 1, 60, 30, 6, 6, 102400, 90000);
  Job secondJob = Job(2, startTime + 5, 120, 100, 8, 8, 102400, 90000);
  Job thirdJob = Job(3, startTime + 5, 100, 95, 8, 4, 102400, 90000);
  Job fourthJob = Job(4, startTime + 5, 90, 50, 8, 6, 102400, 45000);
  Job fifthJob = Job(5, startTime + 8, 80, 40, 6, 6, 102400, 90000);
  jobList.push_back(firstJob);
  jobList.push_back(secondJob);
  jobList.push_back(thirdJob);
  jobList.push_back(fourthJob);
  jobList.push_back(fifthJob);
  return jobList;
}

bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs) 
{
    return (jobList.empty() && jobQueue.empty() && runningJobs.empty());
}

/*
std::vector<Job> buildRandomizedJobs(int jobCount, std::vector<Job> jobList)
{
    
  //  double random = lowLimit + (rand() % higherLimit - 1)
    for(int i = 0; i < jobCount; i++)  
    {
        jobList.push_back(currentJob);

    }
} */


