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

void runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList,  std::time_t startTime)
{
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::cout << "Running the FCFS scheduling algorithm." << std::endl;
    std::time_t currentTime = startTime;    
    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {
        // First check if any jobs are ready to be added to the queue:
        for(long unsigned int i = 0; i < jobList.size(); ++i)
        {
            Job currentJob = jobList.at(i);
            // Check if it is possible to service this request at all: (based on the maximum resources we have available)
            if(!isJobValid(currentJob, nodeList))
            {   // Discard job if infeasible:
                jobList.erase(jobList.begin() + i);
            }
            
            // If the job is ready to submit right now, put it in the queue and remove it from joblist:
            if(currentJob.submitTime == currentTime)
            {
                jobQueue.push_back(currentJob);
                jobList.erase(jobList.begin() + i);    
            }
        
        // Check if any running jobs are finished:
        for(std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
        {
            if(currentTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
            {
                runningJobs.erase(runningJob);
                // Reset node resources: (CPU cores and memory allocated)
                nodeList.at((*runningJob).nodeId).coresAllocated -= (*runningJob).requestedCPUs;
                nodeList.at((*runningJob).nodeId).memoryAllocated -= (*runningJob).requestedMemory;
            }
        }
        
        // Sort the queue based on arrival time to make it first come first serve:
        std::sort(jobQueue.begin(), jobQueue.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.startTime > rhs.startTime;
        });
        
        // Finally, start jobs in the queue as resources permit:
        for(std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeId = checkNodeResources((*waitingJob), nodeList); 
            // If we have a node that is available, assign us to run on it:
            if(selectedNodeId > 0)
            {
                Node selectedNode = nodeList.at(selectedNodeId);
                (*waitingJob).jobStatus = RUNNING;
                selectedNode.coresAllocated += (*waitingJob).requestedCPUs;
                selectedNode.memoryAllocated += (*waitingJob).requestedMemory;
                jobQueue.erase(waitingJob);
            }
            else 
            {
                (*waitingJob).waitTime += 1;
            }
        }
        std::cout << "Running job " << currentJob.jobNum << " with a submit time of: " << currentJob.submitTime << std::endl;
        std::cout << "Running job " << currentJob.jobNum << " with a start time of: " << currentJob.startTime << std::endl;
        std::cout << "Running job " << currentJob.jobNum << " with a requested job runtime of: " << currentJob.requestedRunTime << std::endl;
        // std::cout << "Current time for the SJF algorithm: " << currentTime << std::endl;
        // Increment to the next second: (step-increase in time)
        currentTime++;
        }
    }
}
