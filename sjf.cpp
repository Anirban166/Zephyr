#include "batchScheduler.h"

void runSJF(std::vector<Node> nodeList, std::vector<Job> jobList, std::time_t startTime)
{
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::cout << "Running the SJF scheduling algorithm." << std::endl;
    std::time_t currentTime = startTime;
    int simIteration = 0;
    jobList = verifyJobs(jobList, nodeList);
      
    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {   
        std::cout << "SJF ITERATION: " << simIteration << "\n";

        // First check if any jobs are ready to be added to the queue:
        if(jobList.size()){    
            for(std::vector<Job>::iterator currentJobIter = std::prev(jobList.end()); currentJobIter != std::prev(jobList.begin()); --currentJobIter)
            {
                Job currentJob = *currentJobIter;
                // If the job is ready to submit right now, put it in the queue and remove it from joblist:
                if(currentJob.submitTime == currentTime)
                {
                    currentJob.jobStatus = QUEUED;
                    jobQueue.push_back(currentJob);
                    jobList.erase(currentJobIter);    
                }
            }  
        }
         
        
        if(runningJobs.size()){
            // Check if any running jobs are finished:
            for(std::vector<Job>::iterator runningJob = std::prev(runningJobs.end()); runningJob != std::prev(runningJobs.begin()); --runningJob)
            {
                if(currentTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
                {
                    std::cout << "Job: " << (*runningJob).jobNum << " FINISHED RUNNING ON NODE: " << (*runningJob).nodeId << "\n";
                    runningJobs.erase(runningJob);
                    // Reset node resources: (CPU cores and memory allocated)
                    nodeList.at((*runningJob).nodeId).coresAllocated -= (*runningJob).requestedCPUs;
                    nodeList.at((*runningJob).nodeId).memoryAllocated -= (*runningJob).requestedMemory;
                }
            }
        }
        

        //SORT JOBS FOR SJF SCHEDULING, SORT WITH THE SMALLEST VALUE RUNTIME (highest priority) LAST.
       // std::cout << "Curr time: " << currentTime << " queue before sort.\n"; 
       // printJobs(jobQueue);
        // Sort the queue based on runtime to have the shortest jobs considered first:
        std::sort(jobQueue.begin(), jobQueue.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.requestedRunTime < rhs.requestedRunTime;
        });
        // std::reverse after this to switch the order to descending
        std::cout << "JOB QUEUE SORTED: \n";
        printJobs(jobQueue);
        

        std::vector<Job> waitingList;
        // Finally, start jobs in the queue as resources permit
        for(std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeId = checkNodeResources((*waitingJob), nodeList); 
            // If we have a node that is available, assign us to run on it:
            if(selectedNodeId > -1)
            {
            //     Node selectedNode = nodeList.at(selectedNodeId);
                (*waitingJob).jobStatus = RUNNING;
                (*waitingJob).startTime = currentTime;
                nodeList.at(selectedNodeId).coresAllocated += (*waitingJob).requestedCPUs;
                nodeList.at(selectedNodeId).memoryAllocated += (*waitingJob).requestedMemory;
                //jobQueue.erase(waitingJob);
                Job selectedJob = (*waitingJob);
                selectedJob.nodeId = selectedNodeId;
                runningJobs.push_back(selectedJob);
                std::cout << "Running job " << selectedJob.jobNum << " with a submit time of: " << selectedJob.submitTime << std::endl;
                std::cout << "Running job " << selectedJob.jobNum << " with a start time of: " << selectedJob.startTime << std::endl;
                std::cout << "Running job " << selectedJob.jobNum << " with a requested job runtime of: " << selectedJob.requestedRunTime << std::endl;
            }
            else 
            {
                (*waitingJob).waitTime += 1;
                //Add the rejected job to the waiting list. 
                waitingList.push_back(*waitingJob);
            }
        }
        //Recreate the queue with only the processes that are still waiting. 
        jobQueue = waitingList;
        // std::cout << "Current time for the FCFS algorithm: " << currentTime << std::endl;
        // Increment to the next second: (step-increase in time)
        currentTime++;
        simIteration++;
    }
    
}