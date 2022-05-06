#include "../batchScheduler.h"

Metrics runSJF(std::vector<Node> nodeList, std::vector<Job> jobList, timestamp startTime)
{
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::vector<Job> finalJobList;
    print("Running the SJF scheduling algorithm.\n");
    timestamp currentTime = startTime;
    int simIteration = 0;
    jobList = verifyJobs(jobList, nodeList);
    Metrics sjfMetrics = Metrics("SJF");
    sjfMetrics.totalJobsRun = jobList.size();
      
    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {   
        print("SJF scheduling iteration number: ", simIteration, "\n");
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
        
        if(runningJobs.size())
        {
            // Check if any running jobs are finished:
            for(std::vector<Job>::iterator runningJob = std::prev(runningJobs.end()); runningJob != std::prev(runningJobs.begin()); --runningJob)
            {
                if(currentTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
                {
                    print("Job: ", (*runningJob).jobNum, " finished running on node: ", (*runningJob).nodeID, "\n");
                    (*runningJob).stopTime = currentTime;
                    finalJobList.push_back((*runningJob));
                    
                    // Reset node resources: (CPU cores and memory allocated)
                    nodeList.at((*runningJob).nodeID).coresAllocated -= (*runningJob).requestedCPUs;
                    nodeList.at((*runningJob).nodeID).memoryAllocated -= (*runningJob).requestedMemory;
                    runningJobs.erase(runningJob);
                }
            }
        }
           // Sort jobs in the queue based on the requested run times for SJF:
          // std::cout << "Current time: " << currentTime << " queue before sort.\n"; 
         // printJobs(jobQueue);
        std::sort(jobQueue.begin(), jobQueue.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.requestedRunTime < rhs.requestedRunTime;
        });
        // Reversing items in container to switch the order to descending:
        std::cout << "Sorted job queue: \n";
        printJobs(jobQueue);

        std::vector<Job> waitingList;
        // Finally, start jobs in the queue as resources permit:
        for(std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeID = checkNodeResources((*waitingJob), nodeList); 
            // If we have a node that is available, assign the waiting job to run on it:
            if(selectedNodeID > -1)
            {
                Job selectedJob = (*waitingJob);                
                selectedJob.startTime = currentTime;
                selectedJob.waitTime = currentTime - selectedJob.submitTime;
                
                // Add this job's waiting time to the total time:
                sjfMetrics.totalWaitSum += selectedJob.waitTime;
                sjfMetrics.longestWait = (sjfMetrics.longestWait < selectedJob.waitTime) ? selectedJob.waitTime : sjfMetrics.longestWait;    
                
                // Collect stretch metrics:
                selectedJob.stretch = (double) (selectedJob.waitTime + selectedJob.trueRunTime) / (double) selectedJob.trueRunTime;
                sjfMetrics.totalStretch += selectedJob.stretch;
                sjfMetrics.maxStretch = (sjfMetrics.maxStretch < selectedJob.stretch) ? selectedJob.stretch : sjfMetrics.maxStretch;
                
                // Compute turnaround time and add to total:
                selectedJob.turnAroundTime = (selectedJob.startTime + selectedJob.trueRunTime) - selectedJob.submitTime;
                sjfMetrics.totalturnAroundTime += selectedJob.turnAroundTime;
                sjfMetrics.maxTurnAroundTime = (sjfMetrics.maxTurnAroundTime < selectedJob.turnAroundTime) ? selectedJob.turnAroundTime : sjfMetrics.maxTurnAroundTime;

                // Allocate resources for the waiting job:
                nodeList.at(selectedNodeID).coresAllocated += selectedJob.requestedCPUs;
                nodeList.at(selectedNodeID).memoryAllocated += selectedJob.requestedMemory;
                selectedJob.jobStatus = RUNNING;
                selectedJob.nodeID = selectedNodeID;
                runningJobs.push_back(selectedJob);
                print("Running job ", selectedJob.jobNum, " with a submit time of: ", selectedJob.submitTime, " on node: ", selectedNodeID, "\n");
                print("Running job ", selectedJob.jobNum, " with a start time of: ", selectedJob.startTime, "\n");
                print("Running job ", selectedJob.jobNum, " with a requested job runtime of: ", selectedJob.requestedRunTime, "\n");

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
                sjfMetrics.totalCPUsUsed += numCPUsInUse;
                sjfMetrics.maxCPUsUsed = (numCPUsInUse > sjfMetrics.maxCPUsUsed) ? numCPUsInUse : sjfMetrics.maxCPUsUsed;
                sjfMetrics.totalMemoryUsed += memoryInUse;
                sjfMetrics.maxMemoryUsed = (memoryInUse > sjfMetrics.maxMemoryUsed) ? memoryInUse : sjfMetrics.maxMemoryUsed;    
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
        // std::cout << "Current time for the SJF algorithm: " << currentTime << std::endl;
        // Increment to the next second: (step-increase in time)
        currentTime++;
        simIteration++;
    }
    // Use finalJobList to calculate metrics. 
    
    return sjfMetrics;
}