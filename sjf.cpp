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
        if(waitingJob.requestedCPUs <= (*currentNode).coreCount && waitingJob.requestedMemory <= (*currentNode).memoryAmount)
            return (*currentNode).nodeId;    
    } 
    return -1;   
}

void runSJF(std::vector<Node> nodeList, std::vector<Job> jobList,  std::time_t startTime)
{
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::cout << "Running SJF" << std::endl;
    std::time_t currTime = startTime;    
    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {
        //First check if any jobs are ready to be added to the queue.
        for(long unsigned int i = 0; i < jobList.size(); ++i)
        {
            Job currJob = jobList.at(i);
            //Check if it is possible to service this request at all.
            if(!isJobValid(currJob, nodeList)){
                jobList.erase(jobList.begin()+i);
            }
            
            //If the job is ready to submit right now, put in the queue and remove from joblist.
            if(currJob.submitTime == currTime)
            {
                jobQueue.push_back(currJob);
                jobList.erase(jobList.begin() + i);    
            }
        
        //Then check if any running jobs are finished.
        for(std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
        {
            if(currTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
            {
                runningJobs.erase(runningJob);
                //Reset node resources
                //Reset CPU cores
                nodeList.at((*runningJob).nodeId).coresAllocated -= (*runningJob).requestedCPUs;
                //Reset Memory allocated
                nodeList.at((*runningJob).nodeId).memoryAllocated -= (*runningJob).requestedMemory;
            }
        }
        
        //Sort the queue based on runtime to have the shortest jobs considered first.
        std::sort( jobQueue.begin( ), jobQueue.end( ), [ ]( const auto& lhs, const auto& rhs )
            {
            return lhs.requestedRunTime > rhs.requestedRunTime;
            });
        
        //Finally start jobs in the queue as resources permit.
        for(std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeId = checkNodeResources((*waitingJob), nodeList); 
            //If we have a node that is available, assign us to run on it.
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

        std::cout << "SJF: currTime " << currTime << std::endl;
        
        //Increment to the next second.
        currTime++;
        }
    }
}