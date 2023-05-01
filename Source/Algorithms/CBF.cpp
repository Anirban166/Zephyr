#include "../batchScheduler.h"

Metrics runCBF(std::vector<Node> nodeList, std::vector<Job> jobList, timestamp startTime, int mode)
{
    std::ofstream outputfile("CBFOutput.txt", std::ios::trunc);
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::vector<Job> finalJobList;
    std::vector<Job> reservingJobs;
    timestamp currentTime(startTime);

    if(mode) print("Running the CBF scheduling algorithm.\n");
    outputfile << "Running the CBF scheduling algorithm.\n";
    int simIteration = 0;
    jobList = verifyJobs(jobList, nodeList);
    Metrics cbfMetrics = Metrics("CBF");
    cbfMetrics.totalJobsRun = jobList.size();

    while(!simulationFinished(jobList, jobQueue, runningJobs))
    {
        if(mode) 
        {
            print("CBF scheduling iteration number: ", simIteration, "\n", "Reserving jobs: ");
            printReservedJobs(reservingJobs);
        }    
        outputfile << "CBF scheduling iteration number: " << simIteration << "\n";
        // First check if any jobs are ready to be added to the queue:
        if(jobList.size())
        {
            for(std::vector<Job>::iterator currentJobIter = std::prev(jobList.end()); currentJobIter != std::prev(jobList.begin()); --currentJobIter)
            {
                Job currentJob = *currentJobIter;
                // If the job is ready to be submitted right now, put it in the queue and remove it from the joblist:
                if(currentJob.submitTime == currentTime)
                {
                    outputfile << "Adding job number " << currentJob.jobNum << " to the queue.\n";
                    currentJob.jobStatus = QUEUED;
                    jobQueue.push_back(currentJob);
                    jobList.erase(currentJobIter);
                }
            }
        }
        // Sort jobs based on the submit times for CBF:
        // std::cout << "Current time: " << currentTime << " queue before sort.\n";
        // printJobs(jobQueue);
        std::sort(jobQueue.begin(), jobQueue.end(), [](const auto &lhs, const auto &rhs)
        { return lhs.submitTime > rhs.submitTime; });
        // Reversing items in container to switch the order to descending:
        std::reverse(jobQueue.begin(), jobQueue.end());

        outputfile << "Sorted job queue: ";
        if(mode) printJobs(jobQueue);

        if(runningJobs.size())
        {
            // Check if any running jobs are finished:
            for(std::vector<Job>::iterator runningJob = std::prev(runningJobs.end()); runningJob != std::prev(runningJobs.begin()); --runningJob)
            {
                outputfile << "Time taken by job " << (*runningJob).jobNum << ": " << (currentTime - (*runningJob).startTime) << "\n";

                if(currentTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
                {
                    // print("Job: ", (*runningJob).jobNum, " finished running on node: ", (*runningJob).nodeID, "\n");
                    outputfile << "Job " << (*runningJob).jobNum << " finished running on node: " << (*runningJob).nodeID << "\n";
                    (*runningJob).stopTime = currentTime;

                    finalJobList.push_back((*runningJob));

                    // Reset node resources: (CPU cores and memory allocated)
                    nodeList.at((*runningJob).nodeID).coresAllocated -= (*runningJob).requestedCPUs;
                    nodeList.at((*runningJob).nodeID).memoryAllocated -= (*runningJob).requestedMemory;
                    runningJobs.erase(runningJob);
                }

                // Kill jobs if they go over their requested time.
                else if((currentTime - (*runningJob).startTime) > ((*runningJob).requestedRunTime))
                {
                    outputfile << "Job " << (*runningJob).jobNum << " killed due to time limit on node: " << (*runningJob).nodeID << "\n";
                    (*runningJob).stopTime = currentTime;
                    finalJobList.push_back((*runningJob));

                    // Reset node resources: (CPU cores and memory allocated)
                    nodeList.at((*runningJob).nodeID).coresAllocated -= (*runningJob).requestedCPUs;
                    nodeList.at((*runningJob).nodeID).memoryAllocated -= (*runningJob).requestedMemory;
                    runningJobs.erase(runningJob);
                }
            }
        }

        std::vector<Job> waitingList;
        // Finally, start jobs in the queue as resources permit:
        for(std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeID = checkNodeResources((*waitingJob), nodeList);
            bool backfilling = false;
            // bool canFinishInTime = canFinishBeforeShadowCBF.(shadowTime, (*waitingJob).requestedRunTime, currentTime);
            // bool jobsAreReserving = jobsReserving(waitingList);
            // If we have a node that is available, assign the waiting job to run on it:
            if(selectedNodeID > -1)
            {
                Job selectedJob = (*waitingJob);
                // If we have a job waiting on the shadow time, then we need to see if this one fits before running.
                if(reservingJobs.size() && reservingJobs.at(0).jobNum != selectedJob.jobNum)
                {
                    // Takes a list of reserving jobs and uses the shadow time of the first job in the queue on our node to determine if it can run
                    bool canFinishInTime = canFinishBeforeShadowCBF(runningJobs, (*waitingJob).requestedRunTime, selectedNodeID, currentTime);
                    if(!canFinishInTime)
                    {
                        outputfile << "\nJob " << selectedJob.jobNum << " cannot finish in time as another job is reserving a runtime slot!\n";
                        continue; // to the next job that might fit.
                    }
                    backfilling = true;
                    outputfile << "\nJob " << selectedJob.jobNum << " can be backfilled and finished in time!\n";
                }
                // We are the first job and/or we can fit without waiting. Update the shadow time if we extend past the current one.
                // else if(!canFinishInTime)
                // {
                //    shadowTime = currentTime + selectedJob.requestedRunTime;
                // }

                selectedJob.startTime = currentTime;
                selectedJob.waitTime = currentTime - selectedJob.submitTime;
                selectedJob.nodeID = selectedNodeID;

                // We can no longer reserve a slot if, ya know, we are already using one and running.
                if(std::find_if(reservingJobs.begin(), reservingJobs.end(), [&](Job const &job)
                { return job.jobNum == selectedJob.jobNum; }) != reservingJobs.end())
                {
                    if(mode) print("Selected Job (#", selectedJob.jobNum, ") started removing itself from the queue for reserved jobs!");
                    // reservingJobs.erase(waitingJob);
                    for(std::vector<Job>::iterator reservingJob = reservingJobs.begin(); reservingJob != reservingJobs.end(); ++reservingJob)
                    {
                        if((*reservingJob).jobNum == selectedJob.jobNum)
                        {
                            reservingJobs.erase(reservingJob);
                            break;
                        }
                    }
                    if(mode) print("Selected Job: (#", selectedJob.jobNum, ") removed itself from the queue for reserved jobs!");
                }

                // Notify the next job waiting for our node, if it exists.
                if(reservingJobs.size() && !backfilling)
                {
                    for(std::vector<Job>::iterator reservingJob = reservingJobs.begin(); reservingJob != reservingJobs.end(); ++reservingJob)
                    {
                        if((*reservingJob).nodeID == selectedJob.nodeID)
                        {
                            (*reservingJob).shadowTime = (selectedJob.startTime + selectedJob.requestedRunTime);
                            if(mode) print("Job ", selectedJob.jobNum, " updated shadow time of job ", (*reservingJob).jobNum, "\n");
                            break;
                        }
                    }
                }
                // Add this job's waiting time to the total time:
                cbfMetrics.totalWaitSum += selectedJob.waitTime;
                cbfMetrics.longestWait = (cbfMetrics.longestWait < selectedJob.waitTime) ? selectedJob.waitTime : cbfMetrics.longestWait;

                // Collect stretch metrics:
                selectedJob.stretch = (double)(selectedJob.waitTime + selectedJob.trueRunTime) / (double)selectedJob.trueRunTime;
                cbfMetrics.totalStretch += selectedJob.stretch;
                cbfMetrics.maxStretch = (cbfMetrics.maxStretch < selectedJob.stretch) ? selectedJob.stretch : cbfMetrics.maxStretch;

                // Compute turnaround time and add to total:
                selectedJob.turnAroundTime = (selectedJob.startTime + selectedJob.trueRunTime) - selectedJob.submitTime;
                cbfMetrics.totalturnAroundTime += selectedJob.turnAroundTime;
                cbfMetrics.maxTurnAroundTime = (cbfMetrics.maxTurnAroundTime < selectedJob.turnAroundTime) ? selectedJob.turnAroundTime : cbfMetrics.maxTurnAroundTime;

                // Allocate resources for the waiting job:
                nodeList.at(selectedNodeID).coresAllocated += selectedJob.requestedCPUs;
                nodeList.at(selectedNodeID).memoryAllocated += selectedJob.requestedMemory;
                selectedJob.jobStatus = RUNNING;
                selectedJob.nodeID = selectedNodeID;
                runningJobs.push_back(selectedJob);

                outputfile << "Running job " << selectedJob.jobNum << " with a submit time of: " << selectedJob.submitTime << " seconds on node: " << selectedNodeID << "\n";
                outputfile << "Running job " << selectedJob.jobNum << " with a start time of: " << selectedJob.startTime << " seconds\n";
                outputfile << "Running job " << selectedJob.jobNum << " with a requested job runtime of: " << selectedJob.requestedRunTime << " seconds\n";

                // Calculate the number of cores actually being used, in addition to the requested number:
                int numCPUsInUse = 0;
                unsigned long memoryInUse = 0;
                for(std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
                {
                    Job currRunningJob = (*runningJob);
                    numCPUsInUse += currRunningJob.usedCPUs;
                    memoryInUse += currRunningJob.usedMemory;
                }

                // Sum up and obtain the totals for our metrics:
                cbfMetrics.totalCPUsUsed += numCPUsInUse;
                cbfMetrics.maxCPUsUsed = (numCPUsInUse > cbfMetrics.maxCPUsUsed) ? numCPUsInUse : cbfMetrics.maxCPUsUsed;
                cbfMetrics.totalMemoryUsed += memoryInUse;
                cbfMetrics.maxMemoryUsed = (memoryInUse > cbfMetrics.maxMemoryUsed) ? memoryInUse : cbfMetrics.maxMemoryUsed;
            }
            else
            {
                if(currentTime == (*waitingJob).submitTime)
                {
                    // Get node that can hold this job
                    (*waitingJob).nodeID = isJobValid((*waitingJob), nodeList);
                    timestamp newShadowTime = findShadowTimeFromPreceedingJobs(runningJobs, (*waitingJob).nodeID);
                    (*waitingJob).shadowTime = newShadowTime;
                    reservingJobs.push_back((*waitingJob));
                }
                (*waitingJob).waitTime += 1;
                // Add the rejected job to the waiting list:
                waitingList.push_back(*waitingJob);
            }
        }
        // Recreate the queue with only the processes that are still waiting:
        jobQueue = waitingList;
        // std::cout << "Current time for the CBF algorithm: " << currentTime << std::endl;
        // Increment to the next second: (step-increase in time)
        currentTime++;
        simIteration++;
    }
    // Use finalJobList to calculate metrics.

    outputfile.close();
    return cbfMetrics;
}