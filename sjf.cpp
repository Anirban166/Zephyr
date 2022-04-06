#include "batchScheduler.h"

void runSJF(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime)
{
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::cout << "Running the SJF scheduling algorithm." << std::endl;
    std::time_t currentTime = startTime;
    int simIteration = 0;
    jobList = verifyJobs(jobList, nodeList);
     // If the job is ready to submit right now, put it in the queue and remove it from joblist:
     for(std::vector<Job>::iterator currentJobIter = jobList.end(); currentJobIter != jobList.begin(); --currentJobIter)
     {  
        Job currentJob = *currentJobIter;  
        // If the job is ready to submit right now, put it in the queue and remove it from joblist:
        if(currentJob.submitTime == currentTime)
        {
            jobQueue.push_back(currentJob);
            jobList.erase(currentJobIter);    
        }
     }
     // First check if any jobs are ready to be added to the queue:
        for(std::vector<Job>::iterator currentJobIter = jobList.end(); currentJobIter != jobList.begin(); --currentJobIter)
        {
            Job currentJob = *currentJobIter;
            // Check if it is possible to service this request at all: (based on the maximum resources we have available)
            if(!isJobValid(currentJob, nodeList))
            {   // Discard job if infeasible:
                jobList.erase(currentJobIter);
            }
            
            // If the job is ready to submit right now, put it in the queue and remove it from joblist:
            if(currentJob.submitTime == currentTime)
            {
                jobQueue.push_back(currentJob);
                jobList.erase(currentJobIter);    
            }
        }    
    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {   
        std::cout << "SJF ITERATION: " << simIteration << "\n";
       
        // Check if any running jobs are finished:
        for(std::vector<Job>::iterator runningJob = runningJobs.end(); runningJob != runningJobs.begin(); --runningJob)
        {
            if(currentTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
            {
                runningJobs.erase(runningJob);
                // Reset node resources: (CPU cores and memory allocated)
                nodeList.at((*runningJob).nodeId).coresAllocated -= (*runningJob).requestedCPUs;
                nodeList.at((*runningJob).nodeId).memoryAllocated -= (*runningJob).requestedMemory;
            }
        }

        std::cout << "Curr time: " << currentTime << " queue before sort.\n"; 
        printJobs(jobQueue);
        // Sort the queue based on runtime to have the shortest jobs considered first:
        std::sort(jobQueue.begin(), jobQueue.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.requestedRunTime < rhs.requestedRunTime;
        });
        // std::reverse after this to switch the order to descending
        printJobs(jobQueue);
        
        // Finally, start jobs in the queue as resources permit:
        for(std::vector<Job>::iterator waitingJob = jobQueue.end(); waitingJob != jobQueue.begin(); ++waitingJob)
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
        // std::cout << "Current time for the FCFS algorithm: " << currentTime << std::endl;
        // Increment to the next second: (step-increase in time)
        currentTime++;
        simIteration++;
    }
    
}