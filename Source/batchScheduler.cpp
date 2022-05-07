#include "batchScheduler.h"

auto main(int argc, char *argv[]) -> int
{
    if(argc != 3)
    {
        print("Please enter the following arguments:\n");
        print("The number of jobs to schedule.\n");
        print("The scheduling algorithm to use.\n");
        exit(-1);
    }
    print("The arguments you entered are:\n");
    for(int i = 0; i < argc; ++i)
    {
        print("Argument ", i, ": ", argv[i], "\n");
    }
    // int jobCount = atoi(argv[1]);
    std::string schedulingAlgorithm = argv[2];

    // Get the current time to reference as a start point for the entire simulation:
    time_point<system_clock, seconds> startTimePoint = time_point_cast<seconds>(system_clock::now());
    std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);

    // Metrics to be updated after algorithm runs.
    Metrics algorithmMetrics = Metrics(schedulingAlgorithm);

    // Generate nodes and jobs:
    int nodeCount = 3;
    std::vector<Node> nodeList = buildNodes(nodeCount);
    std::vector<Job> jobList = buildRandomizedJobs(5, startTime);
    // std::vector<Job> jobList = buildPresetJobs(startTime, schedulingAlgorithm);

    // std::vector<Job> jobList = buildRandomizedJobs(jobCount, startTime);
    // Choose algorithm to run based on command line input:
    if (!schedulingAlgorithm.compare("FCFS"))
    {
        finalizeAndOutputMetrics(runFCFS(nodeList, jobList, startTime), "FCFSMetrics.txt");
    }
    else if (!schedulingAlgorithm.compare("SJF"))
    {
        finalizeAndOutputMetrics(runSJF(nodeList, jobList, startTime), "SJFMetrics.txt");
    }
    else if (!schedulingAlgorithm.compare("EASY"))
    {
        finalizeAndOutputMetrics(runEASY(nodeList, jobList, startTime), "EASYMetrics.txt");
    }
    else if (!schedulingAlgorithm.compare("CBF"))
    {
        finalizeAndOutputMetrics(runCBF(nodeList, jobList, startTime), "CBFMetrics.txt");
    }
    else if (!schedulingAlgorithm.compare("ALL"))
    {
        finalizeAndOutputMetrics(runFCFS(nodeList, jobList, startTime), "FCFSMetrics.txt");
        finalizeAndOutputMetrics(runSJF(nodeList, jobList, startTime), "SJFMetrics.txt");
        finalizeAndOutputMetrics(runEASY(nodeList, jobList, startTime), "EASYMetrics.txt");
        finalizeAndOutputMetrics(runCBF(nodeList, jobList, startTime), "CBFMetrics.txt");
    }    
    else
    {
        print("Invalid algorithm name! Current options include FCFS, SJF, EASY, CBF and ALL (to run all four separately).\n");
        exit(-1);
    }
}

std::vector<Node> buildNodes(int nodeCount)
{
    std::vector<Node> nodeList;
    int maxCoresPerNode = 24;
    int maxMemoryPerNode = 102400; // Allocating 100 GiB per node (102400 MiB)
    // for (int i = 0; i < nodeCount; ++i)
    // {
    //   nodeList.push_back(Node(i, maxCoresPerNode, maxMemoryPerNode));
    // }
    nodeList.push_back(Node(0, maxCoresPerNode, maxMemoryPerNode));
    // nodeList.push_back(Node(1, maxCoresPerNode, maxMemoryPerNode));
    // nodeList.push_back(Node(2, 8, maxMemoryPerNode));
    return nodeList;
}

std::vector<Job> buildPresetJobs(timestamp startTime, std::string algorithm)
{
    std::vector<Job> jobList;
    if (!algorithm.compare("FCFS") || !algorithm.compare("SJF"))
    {
        jobList.push_back(Job(0, startTime + 1, 60, 30, 6, 6, 102400, 90000));
        jobList.push_back(Job(1, startTime + 4, 120, 100, 8, 8, 102400, 90000));
        jobList.push_back(Job(2, startTime + 5, 100, 95, 8, 4, 102400, 90000));
        jobList.push_back(Job(3, startTime + 5, 90, 50, 8, 6, 102400, 45000));
        jobList.push_back(Job(4, startTime + 8, 80, 40, 6, 6, 102400, 90000));
    }
    if (!algorithm.compare("EASY") || !algorithm.compare("CBF"))
    {
        jobList.push_back(Job(0, startTime + 1, 60, 30, 6, 6, 90000, 90000));
        jobList.push_back(Job(1, startTime + 3, 120, 100, 8, 8, 80000, 90000));
        jobList.push_back(Job(2, startTime + 6, 50, 95, 8, 4, 35000, 2000));
        jobList.push_back(Job(3, startTime + 6, 20, 50, 8, 6, 2000, 3000));
        jobList.push_back(Job(4, startTime + 8, 80, 40, 6, 6, 102400, 90000));
    }
    return jobList;
}

bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs)
{
    return (jobList.empty() && jobQueue.empty() && runningJobs.empty());
}

void finalizeAndOutputMetrics(Metrics metrics, std::string fileName)
{
    std::ofstream outputfile(fileName, std::ios::trunc);    
    outputfile << "Metrics for simulation:\nAlgorithm used: " << metrics.algorithm << "\n";
    outputfile << "Maximum number of CPUs used simultaneously: " << metrics.maxCPUsUsed << "\nMaximum amount of memory used simultaneously: " << metrics.maxMemoryUsed << " MiB\n";
    metrics.averageCPUsUsed = metrics.totalCPUsUsed / metrics.totalJobsRun;
    metrics.averageMemoryUsed = metrics.totalMemoryUsed / metrics.totalJobsRun;
    outputfile << "Average CPUs in use simultaneously: " << metrics.averageCPUsUsed << "\nAverage amount of memory used simultaneously: " << metrics.averageMemoryUsed << " MiB\n";
    metrics.averageWait = metrics.totalWaitSum / metrics.totalJobsRun;
    outputfile << "Total wait time: " << metrics.totalWaitSum << " seconds\nAverage wait time: " << metrics.averageWait << " seconds\nMaximum wait time: " << metrics.longestWait << " seconds\n";
    metrics.avgStretch = metrics.totalStretch / metrics.totalJobsRun;
    outputfile << "Total stretch: " << metrics.totalStretch << "\nAverage stretch: " << metrics.avgStretch << "\nMaximum stretch: " << metrics.maxStretch << "\n";
    metrics.avgturnAroundTime = metrics.totalturnAroundTime / metrics.totalJobsRun;
    outputfile << "Total Turnaroundtime: " << metrics.totalturnAroundTime << " seconds\n Average Turnaround time: " << metrics.avgturnAroundTime << " seconds\n Maximum turnaround time: " << metrics.maxTurnAroundTime << " seconds\n";
    outputfile.close();
}

std::vector<Job> buildRandomizedJobs(int jobCount, std::time_t startTime)
{
    std::vector<Job> jobList;
    double randomizedSubmitTime = startTime + rangeRNG(1, 10), randomizedRequestedRunTime = rangeRNG(10, 150), randomizedTrueRunTime = rangeRNG(10, 150);
    double randomizedRequestedCPUs = rangeRNG(1, 10), randomizedUsedCPUs = rangeRNG(1, 10), randomizedRequestedMemory = rangeRNG(1000, 102400), randomizedUsedMemory = rangeRNG(1000, 102400);
    for(int i = 0; i < jobCount; i++)
    {
        jobList.push_back(Job(i, randomizedSubmitTime, randomizedRequestedRunTime, randomizedTrueRunTime, 
          randomizedRequestedCPUs, randomizedUsedCPUs, randomizedRequestedMemory, randomizedUsedMemory));
    }
    return jobList;
}

double rangeRNG(double lowerLimit, double upperLimit)
{   // Obtaining a random number from H/W, seeding the generator using it (for run reproducibility) and using a uniform distribution: 
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> uniformDistributionBasedRandomNumber(lowerLimit, upperLimit);    
    return uniformDistributionBasedRandomNumber(generator);
}