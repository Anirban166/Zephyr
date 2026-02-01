#include "batchScheduler.h"

int checkNodeResources(Job waitingJob, std::vector<Node> nodeList)
{
    for(std::vector<Node>::iterator currentNode = nodeList.begin(); currentNode != nodeList.end(); currentNode++)
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
    int nodeID = -1;
    for(std::vector<Node>::iterator currentNode = nodeList.begin(); currentNode != nodeList.end(); ++currentNode)
    {
        if((waitingJob.requestedCPUs <= (*currentNode).coreCount) && (waitingJob.requestedMemory <= (*currentNode).memoryAmount))
        {
            nodeID = (*currentNode).nodeID;
            return nodeID;
        }
    }
    print("Couldn't find a node with desired resources as requested (exceeds the maximum for all nodes)!\n");
    return nodeID;
}

// Takes the target node to run on and determines if we can be backfilled into it based on currently running jobs.
bool canFinishBeforeShadowCBF(std::vector<Job> runningJobs, timestamp reqRuntime, int targetNodeID, timestamp currentTime)
{
    Job preceedingJob = Job(-69, -1, 420694206942069, 69, 420420, 69, 42069, 6942069);
    for(std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
    {
        if((*runningJob).nodeID == targetNodeID)
        {
            preceedingJob = (*runningJob);
            if(((preceedingJob.startTime + preceedingJob.requestedRunTime) - currentTime) < reqRuntime)
            {
                return false;
            }
            return true;
        }
    }
    // Couldn't find any other jobs, this should not happen!
    print("Talk about stuff being broken - canFinishBeforeShadowCBF() found no jobs to backfill into!\n");
    return true;
}

timestamp findShadowTimeFromPreceedingJobs(std::vector<Job> runningJobs, int targetNodeID)
{
    int jobNum = -999;
    // timestamp shadowTime = -999;
    Job preceedingJob = Job(-69, -1, 420694206942069, 69, 420420, 69, 42069, 6942069);
    for(std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
    {
        if((*runningJob).nodeID == targetNodeID)
        {
            jobNum = (*runningJob).jobNum;
            preceedingJob = (*runningJob);
        }
    }
    // If a job is found, use its start + requested runtime to make our shadow time:
    if(jobNum > -1)
    {
        // Job may not be running yet -> need to notify jobs in reserve queue when a job starts running.
        return preceedingJob.startTime + preceedingJob.requestedRunTime;
    }
    return NO_SHADOW_TIME_ASSIGNED; // A job is not running.
}

// Loops through the reserving list and provides a shadow time to the next process that is waiting on this node.
void updateShadowTimeOfNext(std::vector<Job> reservingJobs, Job selectedJob, int targetNodeID)
{
    for(std::vector<Job>::iterator reservingJob = reservingJobs.begin(); reservingJob != reservingJobs.end(); ++reservingJob)
    {
        if((*reservingJob).nodeID == targetNodeID)
        {
            (*reservingJob).shadowTime = (selectedJob.startTime + selectedJob.requestedRunTime);
            return;
        }
    }
}

void printJobs(std::vector<Job> jobs)
{
    int count = 0;
    print("[");
    for(std::vector<Job>::iterator currJob = jobs.begin(); currJob != jobs.end(); ++currJob)
    {
        print("( ", count, "th job in list: ", (*currJob).jobNum, " requires: ", (*currJob).requestedRunTime, " seconds.", " ) ");
        count++;
    }
    print("]\n");
}

void printReservedJobs(std::vector<Job> jobs)
{
    int count = 0;
    print("[");
    for(std::vector<Job>::iterator currJob = jobs.begin(); currJob != jobs.end(); ++currJob)
    {
        print("( ", count, "th job in list: ", (*currJob).jobNum, " requires: ", (*currJob).requestedRunTime, " seconds. Shadow Time: ", (*currJob).shadowTime, " ) ");
        count++;
    }
    print("]\n");
}

std::vector<Job> verifyJobs(std::vector<Job> jobList, std::vector<Node> nodeList)
{
    // First check if any jobs are ready to be added to the queue:
    auto currentJobIter = jobList.begin();
    while (currentJobIter != jobList.end())
    {
        Job currentJob = *currentJobIter;
        // Check if it is possible to service this request at all: (based on the maximum resources we have available)
        if(isJobValid(currentJob, nodeList) == -1)
        {
            print("Erasing job: ", currentJob.jobNum, "\n");
            // Discard job if infeasible:
            currentJobIter = jobList.erase(currentJobIter);
        }
        else
        {
            ++currentJobIter;
        }
    }
    return jobList;
}

bool canFinishBeforeShadow(timestamp shadowTime, timestamp reqRuntime, timestamp currentTime)
{
    return currentTime + reqRuntime <= shadowTime;
}