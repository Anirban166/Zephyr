#include "batchScheduler.h"

Metrics runFCFS(std::vector<Node> nodeList, std::vector<Job> jobList, timestamp startTime)
{
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::vector<Job> finalJobList;
    std::cout << "Running the FCFS scheduling algorithm." << std::endl;
    timestamp currentTime = startTime;
    int simIteration = 0;
    jobList = verifyJobs(jobList, nodeList);
    Metrics fcfsMetrics = Metrics("FCFS");
    fcfsMetrics.totalJobsRun = jobList.size();
      
    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {   
        std::cout << "FCFS scheduling iteration number: " << simIteration << "\n";

        // First check if any jobs are ready to be added to the queue:
        if(jobList.size())
        {    
            for(std::vector<Job>::iterator currentJobIter = std::prev(jobList.end()); currentJobIter != std::prev(jobList.begin()); --currentJobIter)
            {
                Job currentJob = *currentJobIter;
                // If the job is ready to be submitted right now, put it in the queue and remove it from the joblist:
                if(currentJob.submitTime == currentTime)
                {
                    currentJob.jobStatus = QUEUED;
                    jobQueue.push_back(currentJob);
                    jobList.erase(currentJobIter);    
                }
            }  
        }

           // Sort jobs for fcfs scheduling based on the submit times:
          // std::cout << "Current time: " << currentTime << " queue before sort.\n"; 
         // printJobs(jobQueue);
        // Sort the queue based on runtime to have the shortest jobs considered first:
        std::sort(jobQueue.begin(), jobQueue.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.submitTime > rhs.submitTime;
        });
        // Reversing items in container to switch the order to descending:
        std::reverse(jobQueue.begin(), jobQueue.end());
        std::cout << "Sorted job queue: \n";
        printJobs(jobQueue);
         
        if(runningJobs.size())
        {
            // Check if any running jobs are finished:
            for(std::vector<Job>::iterator runningJob = std::prev(runningJobs.end()); runningJob != std::prev(runningJobs.begin()); --runningJob)
            {
                if(currentTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
                {
                    std::cout << "Job: " << (*runningJob).jobNum << " finished running on node: " << (*runningJob).nodeId << "\n";
                    (*runningJob).stopTime = currentTime;
                    
                    finalJobList.push_back((*runningJob));
                    
                    // Reset node resources: (CPU cores and memory allocated)
                    nodeList.at((*runningJob).nodeId).coresAllocated -= (*runningJob).requestedCPUs;
                    nodeList.at((*runningJob).nodeId).memoryAllocated -= (*runningJob).requestedMemory;
                    runningJobs.erase(runningJob);
                }
            }
        }   

        std::vector<Job> waitingList;
        // Finally, start jobs in the queue as resources permit:
        for(std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeId = checkNodeResources((*waitingJob), nodeList); 
            // If we have a node that is available, assign the waiting job to run on it:
            if(selectedNodeId > -1)
            {
                Job selectedJob = (*waitingJob);                
                selectedJob.startTime = currentTime;
                selectedJob.waitTime = currentTime - selectedJob.submitTime;
                
                // Add this job's waiting time to the total time:
                fcfsMetrics.totalWaitSum += selectedJob.waitTime;
                if(fcfsMetrics.longestWait < selectedJob.waitTime)
                    fcfsMetrics.longestWait = selectedJob.waitTime;
                
                // Collect stretch metrics:
                selectedJob.stretch = (double) (selectedJob.waitTime + selectedJob.trueRunTime) / (double) selectedJob.trueRunTime;
                fcfsMetrics.totalStretch += selectedJob.stretch;
                if(fcfsMetrics.maxStretch < selectedJob.stretch)
                    fcfsMetrics.maxStretch = selectedJob.stretch;
                
                // Compute turnaround time and add to total:
                selectedJob.turnAroundTime = (selectedJob.startTime + selectedJob.trueRunTime) - selectedJob.submitTime; // stopTime - submitTime
                fcfsMetrics.totalturnAroundTime += selectedJob.turnAroundTime;
                if(fcfsMetrics.maxTurnAroundTime < selectedJob.turnAroundTime)
                    fcfsMetrics.maxTurnAroundTime = selectedJob.turnAroundTime;

                // Allocate resources for the waiting job:
                nodeList.at(selectedNodeId).coresAllocated += selectedJob.requestedCPUs;
                nodeList.at(selectedNodeId).memoryAllocated += selectedJob.requestedMemory;
                selectedJob.jobStatus = RUNNING;
                selectedJob.nodeId = selectedNodeId;
                runningJobs.push_back(selectedJob);
                std::cout << "Running job " << selectedJob.jobNum << " with a submit time of: " << selectedJob.submitTime << " on node: " << selectedNodeId << std::endl;
                std::cout << "Running job " << selectedJob.jobNum << " with a start time of: " << selectedJob.startTime << std::endl;
                std::cout << "Running job " << selectedJob.jobNum << " with a requested job runtime of: " << selectedJob.requestedRunTime << std::endl;

                // Calculate the core count actually being used, in addition to the requested number:
                int numCPUsInUse = 0;
                unsigned long memoryInUse = 0;
                for(std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
                {
                    Job currRunningJob = (*runningJob);
                    numCPUsInUse += currRunningJob.usedCPUs;
                    memoryInUse += currRunningJob.usedMemory;
                }

                // Sum up and obtain the totals for our metrics:
                fcfsMetrics.totalCPUsUsed += numCPUsInUse;
                if(numCPUsInUse > fcfsMetrics.maxCPUsUsed)
                    fcfsMetrics.maxCPUsUsed = numCPUsInUse;
                    
                fcfsMetrics.totalMemoryUsed += memoryInUse;
                if(memoryInUse > fcfsMetrics.maxMemoryUsed)
                    fcfsMetrics.maxMemoryUsed = memoryInUse;
                    
            }
            else 
            {
                (*waitingJob).waitTime += 1;
                // Add the rejected job to the waiting list: 
                waitingList.push_back(*waitingJob);
            }
        }
        // Recreate the queue with only the processes that are still waiting:
        jobQueue = waitingList;
        // std::cout << "Current time for the FCFS algorithm: " << currentTime << std::endl;
        // Increment to the next second: (step-increase in time)
        currentTime++;
        simIteration++;
    }
    // Use finalJobList to calculate metrics. 
    
    return fcfsMetrics;
}