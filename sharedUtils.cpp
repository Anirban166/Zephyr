#include "batchScheduler.h"

int checkNodeResources(Job waitingJob, std::vector<Node> nodeList)
{
    for(std::vector<Node>::iterator currentNode = nodeList.begin(); currentNode != nodeList.end(); ++currentNode)
    {
        if(waitingJob.requestedCPUs <= (*currentNode).coresAllocated && waitingJob.requestedMemory <= (*currentNode).memoryAllocated)
            return (*currentNode).nodeId;    
    } 
    return -1;   
}

int isJobValid(Job waitingJob, std::vector<Node> nodeList)
{
    for(std::vector<Node>::iterator currentNode = nodeList.begin(); currentNode != nodeList.end(); ++currentNode)
    {
        if(waitingJob.requestedCPUs <= (*currentNode).coreCount && waitingJob.requestedMemory <= (*currentNode).memoryAmount){
            std::cout << "Returning available node with id: " << (*currentNode).nodeId << " for job: " << waitingJob.jobNum << "\n";
            return (*currentNode).nodeId;
        }   
    } 
    std::cout << "Couldn't find a node with desired resources as requested (exceeds the maximum for all nodes)\n";
    return -1;   
}

void printJobs(std::vector<Job> jobs)
{
    int count = 0;
    for(std::vector<Job>::iterator currJob = jobs.begin(); currJob != jobs.end(); ++currJob){
        std::cout << "Job " << count << ": " << (*currJob).jobNum << " start time: " << (*currJob).jobNum;
    }
}

std::vector<Job> verifyJobs(std::vector<Job> jobList, std::vector<Node> nodeList){
     // First check if any jobs are ready to be added to the queue:
    for(std::vector<Job>::iterator currentJobIter = std::prev(jobList.end()); currentJobIter != std::prev(jobList.begin()); --currentJobIter)
    {
        Job currentJob = *currentJobIter;
        // Check if it is possible to service this request at all: (based on the maximum resources we have available)
        if(!isJobValid(currentJob, nodeList))
        {   // Discard job if infeasible:
            jobList.erase(currentJobIter);
        }
    }
    return jobList;
}