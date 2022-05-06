#include "batchScheduler.h"

int checkNodeResources(Job waitingJob, std::vector<Node> nodeList)
{
    for (std::vector<Node>::iterator currentNode = nodeList.begin(); currentNode != nodeList.end(); currentNode++)
    {
        Node currNodeObj = (*currentNode);
        int coresAvailable = currNodeObj.coreCount - currNodeObj.coresAllocated;
        int memoryAvailable = currNodeObj.memoryAmount - currNodeObj.memoryAllocated;

        if ((waitingJob.requestedCPUs <= coresAvailable) && (waitingJob.requestedMemory <= memoryAvailable))
            return (*currentNode).nodeID;
    }
    return -1;
}

int isJobValid(Job waitingJob, std::vector<Node> nodeList)
{
    for (std::vector<Node>::iterator currentNode = nodeList.begin(); currentNode != nodeList.end(); ++currentNode)
    {
        if ((waitingJob.requestedCPUs <= (*currentNode).coreCount) && (waitingJob.requestedMemory <= (*currentNode).memoryAmount))
        {
            print("Returning available node with id: ", (*currentNode).nodeID, " for job: ", waitingJob.jobNum, "\n");
            return (*currentNode).nodeID;
        }
    }
    print("Couldn't find a node with desired resources as requested (exceeds the maximum for all nodes)!\n");
    return -1;
}

void printJobs(std::vector<Job> jobs)
{
    int count = 0;
    print("[");
    for (std::vector<Job>::iterator currJob = jobs.begin(); currJob != jobs.end(); ++currJob)
    {
        print(count, "th job in list: ", (*currJob).jobNum, " requires: ", (*currJob).requestedRunTime, " seconds,");
        count++;
    }
    print("]\n");
}

void printJobsToFile(std::vector<Job> jobs, std::ostream file)
{
    int count = 0;
    file << "[";
    for (std::vector<Job>::iterator currJob = jobs.begin(); currJob != jobs.end(); ++currJob)
    {
        file << count << "th job in list: " << (*currJob).jobNum << " requires: " << (*currJob).requestedRunTime << " seconds,";
        count++;
    }
    file << "]\n";
}

std::vector<Job> verifyJobs(std::vector<Job> jobList, std::vector<Node> nodeList)
{
    // First check if any jobs are ready to be added to the queue:
    for (std::vector<Job>::iterator currentJobIter = std::prev(jobList.end()); currentJobIter != std::prev(jobList.begin()); --currentJobIter)
    {
        Job currentJob = *currentJobIter;
        // Check if it is possible to service this request at all: (based on the maximum resources we have available)
        if (isJobValid(currentJob, nodeList) == -1)
        {
            print("Erasing job: ", currentJob.jobNum, "\n");
            // Discard job if infeasible:
            jobList.erase(currentJobIter);
        }
    }
    return jobList;
}

bool canFinishBeforeShadow(timestamp shadowTime, timestamp reqRuntime, timestamp currentTime)
{
    return currentTime + reqRuntime <= shadowTime;
}