#include "batchScheduler.h"

auto main(int argc, char* argv[]) -> int
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

    // int jobCount = atoi(argv[1]);
    std::string schedulingAlgorithm = argv[2];
    
    // Get the current time to reference as a start point for the entire simulation
    time_point<system_clock, seconds> startTimePoint = time_point_cast<seconds>(system_clock::now());
    std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);

    // Generate nodes and jobs:
    int nodeCount = 3;    
    std::vector<Node> nodeList = buildNodes(nodeCount);
    std::vector<Job> jobList = buildPresetJobs(startTime);
    // std::vector<Job> jobList = buildRandomizedJobs(jobCount, startTime);    
    
    // Choose algorithm to run based on command line input:
    if(!schedulingAlgorithm.compare("FCFS")) 
    { 
        runFCFS(nodeList, jobList, startTime);
    }
    else if(!schedulingAlgorithm.compare("SJF"))
    {
        runSJF(nodeList, jobList, startTime);
    }
}

std::vector<Node> buildNodes(int nodeCount)
{
    std::vector<Node> nodeList;
    int maxCoresPerNode = 24;
    int maxMemoryPerNode = 102400; // Allocating 100 GiB per node (102400 MiB)
    for(int i = 0; i < nodeCount; ++i)
    {
        nodeList.push_back(Node(i, maxCoresPerNode, maxMemoryPerNode));
    }
    return nodeList;
}

std::vector<Job> buildPresetJobs(std::time_t startTime)
{   
  std::vector<Job> jobList;
  jobList.push_back(Job(1, startTime + 1, 60, 30, 6, 6, 102400, 90000));
  jobList.push_back(Job(2, startTime + 5, 120, 100, 8, 8, 102400, 90000));
  jobList.push_back(Job(3, startTime + 5, 100, 95, 8, 4, 102400, 90000));
  jobList.push_back(Job(4, startTime + 5, 90, 50, 8, 6, 102400, 45000));
  jobList.push_back(Job(5, startTime + 8, 80, 40, 6, 6, 102400, 90000));
  return jobList;
}

bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs) 
{
    return (jobList.empty() && jobQueue.empty() && runningJobs.empty());
}

/*
std::vector<Job> buildRandomizedJobs(int jobCount, std::time_t startTime)
{
    // double random = lowLimit + (rand() % higherLimit - 1)
    // randomOnetoTen, randomOnetoSixty, randomOneto105000
    for(int i = 0; i < jobCount; i++)  
    {
        jobList.push_back(Job(i, startTime + randomOnetoTen, randomOnetoSixty, randomOnetoSixty, 
                              randomOnetoTen, randomOnetoTen, randomOneto105000, randomOneto105000));
        // Randomized parameters in order: (excluding jobNum i.e.)
        // submitTime, requestedRunTime, trueRunTime, requestedCPUs, usedCPUs, requestedMemory, usedMemory                      
    }
} */