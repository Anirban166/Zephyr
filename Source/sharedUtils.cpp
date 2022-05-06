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

// bool jobsReserving(std::vector<Job> jobQueue){
//    for (std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob) {

//   }
//}

// Takes the target node to run on and determines if we can be backfilled into it based on currently running jobs.
bool canFinishBeforeShadowCBF(std::vector<Job> runningJobs, timestamp reqRuntime, int targetNodeId, timestamp currentTime)
{
    Job preceedingJob = Job(-69, -1, 6000000000000, 69, 343324, 78, 90239, 5646346);
    for (std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
    {
        if ((*runningJob).nodeID == targetNodeId)
        {
            preceedingJob = (*runningJob);
            if (((preceedingJob.startTime + preceedingJob.requestedRunTime) - currentTime) < reqRuntime)
            {
                return false;
            }
            return true;
        }
    }
    // Couldn't find any other jobs, this should not happen!
    print("THIS SHOULD NOT HAPPEN: canFinishBeforeShadowCBF found no jobs to backfill into!\n");
    return true;
}

timestamp findShadowTimeFromPreceedingJobs(std::vector<Job> runningJobs, int targetNodeId)
{
    int jobNum = -999;
    // timestamp shadowTime = -999;
    Job preceedingJob = Job(-69, -1, 6000000000000, 69, 343324, 78, 90239, 5646346);
    for (std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
    {
        if ((*runningJob).nodeID == targetNodeId)
        {
            jobNum = (*runningJob).jobNum;
            preceedingJob = (*runningJob);
        }
    }
    // If we found a job use its times to make our shadow time
    if (jobNum > -1)
    {
        // JOB MAY NOT BE RUNNING YET, NEED TO NOTIFY JOBS IN RESERVE QUEUE WHEN A JOB STARTS RUNNING。
        return preceedingJob.startTime + preceedingJob.requestedRunTime;
    }
    // A job is not running
    return NO_SHADOW_TIME_ASSIGNED;
}

// Loops through the reserving list and provides a shadow time to the next process that is waiting on this node.
void updateShadowTimeOfNext(std::vector<Job> reservingJobs, Job selectedJob, int targetNodeId)
{
    // int jobNum = -999;
    // timestamp shadowTime = -999;
    // Job nextJob = Job(-69, -1, 6000000000000, 69, 343324, 78, 90239, 5646346);
    for (std::vector<Job>::iterator reservingJob = reservingJobs.begin(); reservingJob != reservingJobs.end(); ++reservingJob)
    {
        if ((*reservingJob).nodeID == targetNodeId)
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