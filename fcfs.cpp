#include "batchScheduler.h"

void runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime)
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
                currentJob.jobStatus = QUEUED;
                jobQueue.push_back(currentJob);
                jobList.erase(jobList.begin() + i);    
            }
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
     
        // Finally, start jobs in the queue as resources permit:
        for(std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeId = checkNodeResources((*waitingJob), nodeList); 
            // If we have a node that is available, assign us to run on it:
            if(selectedNodeId > -1)
            {
                Node selectedNode = nodeList.at(selectedNodeId);
                (*waitingJob).jobStatus = RUNNING;
                (*waitingJob).startTime = currentTime;
                selectedNode.coresAllocated += (*waitingJob).requestedCPUs;
                selectedNode.memoryAllocated += (*waitingJob).requestedMemory;
                jobQueue.erase(waitingJob);
                Job selectedJob = (*waitingJob);
                std::cout << "Running job " << selectedJob.jobNum << " with a submit time of: " << selectedJob.submitTime << std::endl;
                std::cout << "Running job " << selectedJob.jobNum << " with a start time of: " << selectedJob.startTime << std::endl;
                std::cout << "Running job " << selectedJob.jobNum << " with a requested job runtime of: " << selectedJob.requestedRunTime << std::endl;
            }
            else 
            {
                (*waitingJob).waitTime += 1;
            }
        }
            // std::cout << "Current time for the SJF algorithm: " << currentTime << std::endl;
            // Increment to the next second: (step-increase in time)
            currentTime++;
        }
}

