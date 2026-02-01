#include "batchScheduler.h"

double rangeRNG(double lowerLimit, double upperLimit)
{
    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<> uniformDistributionBasedRandomNumber(lowerLimit, upperLimit);
    return uniformDistributionBasedRandomNumber(generator);
}

std::vector<Node> buildNodes(std::string nodeSource)
{
    std::vector<Node> nodeList;
    
    if(nodeSource == "DEFAULT") 
    {
        int maxCoresPerNode = 24;
        int maxMemoryPerNode = 102400;
        nodeList.push_back(Node(0, maxCoresPerNode, maxMemoryPerNode));
        nodeList.push_back(Node(1, 8, 30000));
        return nodeList;
    }

    std::ifstream file(nodeSource);
    std::string line;

    if(!file.is_open())
    {
        print("Error: Could not open node file: ", nodeSource, "\n");
        return nodeList;
    }

    while(std::getline(file, line)) 
    {
        if(line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> seglist;
        while(std::getline(ss, segment, ',')) seglist.push_back(segment);

        if(seglist.size() >= 3) 
        {
            try 
            {
                auto getInt = [&](int idx) { return std::stoi(seglist[idx]); };
                nodeList.push_back(Node(getInt(0), getInt(1), getInt(2)));
            } catch(...) { print("Malformed node line: ", line, "\n"); }
        }
    }
    return nodeList;
}

std::vector<Job> buildPresetJobs(std::string filename, timestamp startTime)
{
    std::vector<Job> jobList;
    std::ifstream file(filename);

    if(!file.is_open()) 
    {
        print("Error: Could not open job file: ", filename, "\n");
        return jobList;
    }
    
    print("Loading jobs from: ", filename, "\n");
    std::string line;

    while(std::getline(file, line)) 
    {
        if(line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> seglist;
        while(std::getline(ss, segment, ',')) seglist.push_back(segment);

        if(seglist.size() >= 8) 
        {
            try {
                // Helper lambdas to clean up syntax and avoid repetitive std::stoi/stod calls.
                auto getInt = [&](int idx) { return std::stoi(seglist[idx]); };
                auto getDbl = [&](int idx) { return std::stod(seglist[idx]); };

                jobList.push_back(Job(
                    getInt(0),              // JobID
                    startTime + getInt(1),  // SubmitTime
                    getInt(2),              // ReqTime
                    getInt(3),              // TrueTime
                    getDbl(4),              // ReqCPU
                    getDbl(5),              // UsedCPU
                    getDbl(6),              // ReqMem
                    getDbl(7)               // UsedMem
                ));
            } catch(...) { print("Malformed job line: ", line, "\n"); }
        }
    }
    return jobList;
}

std::vector<Job> buildRandomizedJobs(int jobCount, std::time_t startTime)
{
    std::vector<Job> jobList;
    for(int i = 0; i < jobCount; i++)
    {
        double randomizedSubmitTime = startTime + rangeRNG(1, 10); 
        double randomizedRequestedRunTime = rangeRNG(10, 150);
        double randomizedTrueRunTime = rangeRNG(10, 150);
        double randomizedRequestedCPUs = rangeRNG(1, 10);
        double randomizedUsedCPUs = rangeRNG(1, 10);
        double randomizedRequestedMemory = rangeRNG(1000, 102400); 
        double randomizedUsedMemory = rangeRNG(1000, 102400);
        jobList.push_back(Job(i, randomizedSubmitTime, randomizedRequestedRunTime, randomizedTrueRunTime, randomizedRequestedCPUs, randomizedUsedCPUs, randomizedRequestedMemory, randomizedUsedMemory));
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

    if(!outputfile.is_open()) 
    {
        print("Error opening output file: ", fileName, "\n");
        return;
    }
    
    outputfile << "=======================";
    outputfile << "\nMetrics for simulation:";
    outputfile << "\n=======================";
    outputfile << "\nAlgorithm used: " << metrics.algorithm << "\n";
    outputfile << "Maximum number of CPUs used simultaneously: " << metrics.maxCPUsUsed << "\nMaximum amount of memory used simultaneously: " << metrics.maxMemoryUsed << " MiB\n";
    
    if(metrics.totalJobsRun > 0) 
    {
        metrics.averageCPUsUsed = metrics.totalCPUsUsed / metrics.totalJobsRun;
        metrics.averageMemoryUsed = metrics.totalMemoryUsed / metrics.totalJobsRun;
        metrics.averageWait = metrics.totalWaitSum / metrics.totalJobsRun;
        metrics.avgStretch = metrics.totalStretch / metrics.totalJobsRun;
        metrics.avgturnAroundTime = metrics.totalturnAroundTime / metrics.totalJobsRun;
    }
    
    outputfile << "Average CPUs in use simultaneously: " << metrics.averageCPUsUsed << "\nAverage amount of memory used simultaneously: " << metrics.averageMemoryUsed << " MiB\n";
    outputfile << "Total wait time: " << metrics.totalWaitSum << " seconds\nAverage wait time: " << metrics.averageWait << " seconds\nMaximum wait time: " << metrics.longestWait << " seconds\n";
    outputfile << "Total stretch: " << metrics.totalStretch << "\nAverage stretch: " << metrics.avgStretch << "\nMaximum stretch: " << metrics.maxStretch << "\n";
    outputfile << "Total Turnaroundtime: " << metrics.totalturnAroundTime << " seconds\nAverage Turnaround time: " << metrics.avgturnAroundTime << " seconds\nMaximum turnaround time: " << metrics.maxTurnAroundTime << " seconds\n";
    outputfile.close();
}

auto main(int argc, char *argv[]) -> int
{
    if(argc != 5)
    {
        print("Incorrect arguments.\n");
        print("Usage: ", argv[0], " <JOB_SOURCE> <NODE_SOURCE> <ALGO> <MODE>\n");
        print("Example: ./test.bin 10 DEFAULT CBF 0\n");
        exit(-1);
    }
    print("The arguments you entered are:\n");
    for(int i = 0; i < argc; ++i)
    {
        print("Argument ", i, ": ", argv[i], "\n");
    }

    std::string jobSource = argv[1];
    std::string nodeSource = argv[2];
    std::string schedulingAlgorithm = argv[3];
    int mode = 0;
    
    try 
    {
        mode = std::stoi(argv[4]);
    } catch (...) 
    {
        print("Error: Mode must be an integer.\n");
        exit(-1);
    }

    time_point<system_clock, seconds> startTimePoint = time_point_cast<seconds>(system_clock::now());
    std::time_t startTime = std::chrono::system_clock::to_time_t(startTimePoint);
    Metrics algorithmMetrics = Metrics(schedulingAlgorithm);
    std::vector<Node> nodeList = buildNodes(nodeSource);
    std::vector<Job> jobList;

    // Check if jobSource is a number (random mode) or a file (preset mode):
    bool isNumeric = !jobSource.empty() && std::all_of(jobSource.begin(), jobSource.end(), ::isdigit);
    
    if(isNumeric) 
    {
        int jobCount = std::stoi(jobSource);
        jobList = buildRandomizedJobs(jobCount, startTime);
    } 
    else 
    {
        jobList = buildPresetJobs(jobSource, startTime);
    }

    if(jobList.empty() || nodeList.empty()) 
    {
        print("Error: Job list or Node list is empty. Exiting.\n");
        exit(-1);
    }

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