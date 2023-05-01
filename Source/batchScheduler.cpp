#include "batchScheduler.h"

auto main(int argc, char *argv[]) -> int
{
    if(argc != 4)
    {
        print("Incorrect arguments. Please enter the following:\nThe number of jobs to schedule.\nThe scheduling algorithm to use.\nMode: use 0 for standard use, or 1 for debugging.\n");
        print("Example: ./test.bin 10 CBF 0\n");
        exit(-1);
    }
    print("The arguments you entered are:\n");
    for(int i = 0; i < argc; ++i)
    {
        print("Argument ", i, ": ", argv[i], "\n");
    }
    int jobCount = atoi(argv[1]);
    std::string schedulingAlgorithm = argv[2];
    int mode = atoi(argv[3]);

    // Get the current time to reference as a start point for the entire simulation:
    time_point<system_clock, seconds> startTimePoint = time_point_cast<seconds>(system_clock::now());
    std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);

    // Metrics to be updated after algorithm runs.
    Metrics algorithmMetrics = Metrics(schedulingAlgorithm);

    // Generate nodes and jobs:
    // The nodeCount is fixed to replicate a computing cluster. No randomization here, only jobs are randomized.
    int nodeCount = 3;
    std::vector<Node> nodeList = buildNodes(nodeCount);
    // Uncomment this (and comment out line 33) to run some preset jobs: (change them as per your interest in the corresponding function below!)
    // std::vector<Job> jobList = buildPresetJobs(startTime, schedulingAlgorithm);
    std::vector<Job> jobList = buildRandomizedJobs(jobCount, startTime);

    // Choose algorithm to run based on command line input:
    if(!schedulingAlgorithm.compare("FCFS"))
    {
        finalizeAndOutputMetrics(runFCFS(nodeList, jobList, startTime, mode), "FCFSMetrics.txt");
    }
    else if(!schedulingAlgorithm.compare("SJF"))
    {
        finalizeAndOutputMetrics(runSJF(nodeList, jobList, startTime, mode), "SJFMetrics.txt");
    }
    else if(!schedulingAlgorithm.compare("EASY"))
    {
        finalizeAndOutputMetrics(runEASY(nodeList, jobList, startTime, mode), "EASYMetrics.txt");
    }
    else if(!schedulingAlgorithm.compare("CBF"))
    {
        finalizeAndOutputMetrics(runCBF(nodeList, jobList, startTime, mode), "CBFMetrics.txt");
    }
    else if(!schedulingAlgorithm.compare("ALL"))
    {
        finalizeAndOutputMetrics(runSJF(nodeList, jobList, startTime, mode), "SJFMetrics.txt");
        finalizeAndOutputMetrics(runFCFS(nodeList, jobList, startTime, mode), "FCFSMetrics.txt");        
        finalizeAndOutputMetrics(runEASY(nodeList, jobList, startTime, mode), "EASYMetrics.txt");
        finalizeAndOutputMetrics(runCBF(nodeList, jobList, startTime, mode), "CBFMetrics.txt");
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

    // Add and customize nodes as per your requirements:
    nodeList.push_back(Node(0, maxCoresPerNode, maxMemoryPerNode));
    nodeList.push_back(Node(1, 8, 30000));
    return nodeList;
}

std::vector<Job> buildPresetJobs(timestamp startTime, std::string algorithm)
{
    std::vector<Job> jobList;
    jobList.push_back(Job(0, startTime + 1, 60, 30, 6, 6, 102400, 90000));
    jobList.push_back(Job(1, startTime + 4, 120, 100, 8, 8, 5000, 10000));
    jobList.push_back(Job(2, startTime + 5, 100, 95, 8, 4, 102400, 20000));
    jobList.push_back(Job(3, startTime + 5, 90, 50, 8, 6, 30000, 45000));
    jobList.push_back(Job(4, startTime + 8, 80, 40, 6, 6, 80000, 90000));
    jobList.push_back(Job(5, startTime + 1, 60, 30, 6, 6, 90000, 70000));
    jobList.push_back(Job(6, startTime + 3, 120, 100, 8, 8, 80000, 90000));
    jobList.push_back(Job(7, startTime + 6, 50, 95, 8, 4, 35000, 2000));
    jobList.push_back(Job(8, startTime + 9, 20, 50, 8, 6, 2000, 3000));
    jobList.push_back(Job(9, startTime + 8, 80, 40, 6, 6, 30000, 90000));
    return jobList;
}

bool simulationFinished(std::vector<Job> jobList, std::vector<Job> jobQueue, std::vector<Job> runningJobs)
{
    return (jobList.empty() && jobQueue.empty() && runningJobs.empty());
}

void finalizeAndOutputMetrics(Metrics metrics, std::string fileName)
{
    std::ofstream outputfile(fileName, std::ios::trunc);
    outputfile << "=======================";
    outputfile << "\nMetrics for simulation:";
    outputfile << "\n=======================";
    outputfile << "\nAlgorithm used: " << metrics.algorithm << "\n";
    outputfile << "Maximum number of CPUs used simultaneously: " << metrics.maxCPUsUsed << "\nMaximum amount of memory used simultaneously: " << metrics.maxMemoryUsed << " MiB\n";
    metrics.averageCPUsUsed = metrics.totalCPUsUsed / metrics.totalJobsRun;
    metrics.averageMemoryUsed = metrics.totalMemoryUsed / metrics.totalJobsRun;
    outputfile << "Average CPUs in use simultaneously: " << metrics.averageCPUsUsed << "\nAverage amount of memory used simultaneously: " << metrics.averageMemoryUsed << " MiB\n";
    metrics.averageWait = metrics.totalWaitSum / metrics.totalJobsRun;
    outputfile << "Total wait time: " << metrics.totalWaitSum << " seconds\nAverage wait time: " << metrics.averageWait << " seconds\nMaximum wait time: " << metrics.longestWait << " seconds\n";
    metrics.avgStretch = metrics.totalStretch / metrics.totalJobsRun;
    outputfile << "Total stretch: " << metrics.totalStretch << "\nAverage stretch: " << metrics.avgStretch << "\nMaximum stretch: " << metrics.maxStretch << "\n";
    metrics.avgturnAroundTime = metrics.totalturnAroundTime / metrics.totalJobsRun;
    outputfile << "Total Turnaroundtime: " << metrics.totalturnAroundTime << " seconds\nAverage Turnaround time: " << metrics.avgturnAroundTime << " seconds\nMaximum turnaround time: " << metrics.maxTurnAroundTime << " seconds\n";
    outputfile.close();
}

std::vector<Job> buildRandomizedJobs(int jobCount, std::time_t startTime)
{
    std::vector<Job> jobList;
    for(int i = 0; i < jobCount; i++)
    {
        double randomizedSubmitTime = startTime + rangeRNG(1, 10), randomizedRequestedRunTime = rangeRNG(10, 150), randomizedTrueRunTime = rangeRNG(10, 150);
        double randomizedRequestedCPUs = rangeRNG(1, 10), randomizedUsedCPUs = rangeRNG(1, 10), randomizedRequestedMemory = rangeRNG(1000, 102400), randomizedUsedMemory = rangeRNG(1000, 102400);
        jobList.push_back(Job(i, randomizedSubmitTime, randomizedRequestedRunTime, randomizedTrueRunTime, randomizedRequestedCPUs, randomizedUsedCPUs, randomizedRequestedMemory, randomizedUsedMemory));
    }
    return jobList;
}

double rangeRNG(double lowerLimit, double upperLimit)
{   // Obtaining a random number (uniform distribution) from hardware and seeding the generator using it: (for run reproducibility)
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> uniformDistributionBasedRandomNumber(lowerLimit, upperLimit);
    return uniformDistributionBasedRandomNumber(generator);
}