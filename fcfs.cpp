#include "batchScheduler.h"

void runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime)
{
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::cout << "Running FCFS" << std::endl;
    std::time_t currTime = startTime;     
    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {
        std::cout << "FCFS: currTime " << currTime << std::endl;
        //Increment to the next second.
        currTime++;
    }
}
