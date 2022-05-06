#include "../batchScheduler.h"

Metrics runEASY(std::vector<Node> nodeList, std::vector<Job> jobList, timestamp startTime)
{
    std::ofstream outputfile("EASYOutput.txt", std::ios::trunc);
    const int NO_JOB_RESERVING = -1;
    int reservedJobNum = NO_JOB_RESERVING;
    std::vector<Job> jobQueue;
    std::vector<Job> runningJobs;
    std::vector<Job> finalJobList;
    timestamp shadowTime = startTime;
    timestamp currentTime = startTime;
    print("Running the EASY scheduling algorithm.\n");
    outputfile << "Running the EASY scheduling algorithm.\n";
    int simIteration = 0;
    jobList = verifyJobs(jobList, nodeList);
    Metrics easyMetrics = Metrics("EASY");
    easyMetrics.totalJobsRun = jobList.size();

    while (!simulationFinished(jobList, jobQueue, runningJobs))
    {

        print("EASY scheduling iteration number: ", simIteration, "\n", "ReservedJobNum: ", reservedJobNum, "\n");
        outputfile << "EASY scheduling iteration number: " << simIteration << "\n"
                   << "ReservedJobNum: " << reservedJobNum << "\n";
        //  First check if any jobs are ready to be added to the queue:
        if (jobList.size())
        {
            for (std::vector<Job>::iterator currentJobIter = std::prev(jobList.end()); currentJobIter != std::prev(jobList.begin()); --currentJobIter)
            {
                Job currentJob = *currentJobIter;
                // If the job is ready to be submitted right now, put it in the queue and remove it from the joblist:
                if (currentJob.submitTime == currentTime)
                {
                    outputfile << "Adding job num: " << currentJob.jobNum << " to queue\n";
                    currentJob.jobStatus = QUEUED;
                    jobQueue.push_back(currentJob);
                    jobList.erase(currentJobIter);
                }
            }
        }
        // Sort jobs based on the submit times for EASY:
        // std::cout << "Current time: " << currentTime << " queue before sort.\n";
        // printJobs(jobQueue);
        std::sort(jobQueue.begin(), jobQueue.end(), [](const auto &lhs, const auto &rhs)
                  { return lhs.submitTime > rhs.submitTime; });
        // Reversing items in container to switch the order to descending:
        std::reverse(jobQueue.begin(), jobQueue.end());

        print("Sorted job queue:\n");
        outputfile << "Sorted job queue: ";
        printJobs(jobQueue);

        if (runningJobs.size())
        {
            // Check if any running jobs are finished:
            for (std::vector<Job>::iterator runningJob = std::prev(runningJobs.end()); runningJob != std::prev(runningJobs.begin()); --runningJob)
            {
                // if ((*runningJob).jobNum == 2)
                outputfile << "Time taken by job " << (*runningJob).jobNum << ": " << (currentTime - (*runningJob).startTime) << "\n";

                if (currentTime == ((*runningJob).startTime + (*runningJob).trueRunTime))
                {
                    // print("Job: ", (*runningJob).jobNum, " finished running on node: ", (*runningJob).nodeID, "\n");
                    outputfile << "Job: " << (*runningJob).jobNum << " finished running on node: " << (*runningJob).nodeID << "\n";
                    // Reset resources
                    (*runningJob).stopTime = currentTime;
                    // Reset the waiting job if it finishes.
                    if (reservedJobNum == (*runningJob).jobNum)
                    {
                        reservedJobNum = NO_JOB_RESERVING;
                    }
                    finalJobList.push_back((*runningJob));

                    // Reset node resources: (CPU cores and memory allocated)
                    nodeList.at((*runningJob).nodeID).coresAllocated -= (*runningJob).requestedCPUs;
                    nodeList.at((*runningJob).nodeID).memoryAllocated -= (*runningJob).requestedMemory;
                    runningJobs.erase(runningJob);
                }

                // Kill jobs if they go over their requested time.
                else if ((currentTime - (*runningJob).startTime) > ((*runningJob).requestedRunTime))
                {
                    outputfile << "Job: " << (*runningJob).jobNum << " killed due to time limit on node: " << (*runningJob).nodeID << "\n";
                    // Reset resources
                    (*runningJob).stopTime = currentTime;
                    // Reset the waiting job if it finishes.
                    if (reservedJobNum == (*runningJob).jobNum)
                    {
                        reservedJobNum = NO_JOB_RESERVING;
                    }
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
        for (std::vector<Job>::iterator waitingJob = jobQueue.begin(); waitingJob != jobQueue.end(); ++waitingJob)
        {
            int selectedNodeID = checkNodeResources((*waitingJob), nodeList);
            bool canFinishInTime = canFinishBeforeShadow(shadowTime, (*waitingJob).requestedRunTime, currentTime);
            // If we have a node that is available, assign the waiting job to run on it:
            if (selectedNodeID > -1)
            {
                Job selectedJob = (*waitingJob);
                // If we have a job waiting on the shadow time, then we need to see if this one fits before running.
                if (reservedJobNum != NO_JOB_RESERVING && reservedJobNum != selectedJob.jobNum)
                {
                    // print("Job is reserving!", reservedJobNum, "\n");

                    if (!canFinishInTime)
                    {
                        // print("=====================\nJob ", selectedJob.jobNum, " Cannot finish in time while job: ", reservedJobNum, " is reserving a runtime slot!\n=====================\n");
                        outputfile << "=====================\nJob " << selectedJob.jobNum << " Cannot finish in time while job: " << reservedJobNum << " is reserving a runtime slot!\n=====================\n";
                        continue; // to the next job that might fit.
                    }
                    outputfile << "\n=====================\nJob " << selectedJob.jobNum << " Can finish in time while job: " << reservedJobNum << " is reserving a runtime slot!\n=====================\n";
                }
                // We are the first job and/or we can fit without waiting and are the first. Update the shadow time if we extend past the current one/
                else if (!canFinishInTime)
                {
                    shadowTime = currentTime + selectedJob.requestedRunTime;
                }

                selectedJob.startTime = currentTime;
                selectedJob.waitTime = currentTime - selectedJob.submitTime;

                // Add this job's waiting time to the total time:
                easyMetrics.totalWaitSum += selectedJob.waitTime;
                easyMetrics.longestWait = (easyMetrics.longestWait < selectedJob.waitTime) ? selectedJob.waitTime : easyMetrics.longestWait;

                // Collect stretch metrics:
                selectedJob.stretch = (double)(selectedJob.waitTime + selectedJob.trueRunTime) / (double)selectedJob.trueRunTime;
                easyMetrics.totalStretch += selectedJob.stretch;
                easyMetrics.maxStretch = (easyMetrics.maxStretch < selectedJob.stretch) ? selectedJob.stretch : easyMetrics.maxStretch;

                // Compute turnaround time and add to total:
                selectedJob.turnAroundTime = (selectedJob.startTime + selectedJob.trueRunTime) - selectedJob.submitTime;
                easyMetrics.totalturnAroundTime += selectedJob.turnAroundTime;
                easyMetrics.maxTurnAroundTime = (easyMetrics.maxTurnAroundTime < selectedJob.turnAroundTime) ? selectedJob.turnAroundTime : easyMetrics.maxTurnAroundTime;

                // Allocate resources for the waiting job:
                nodeList.at(selectedNodeID).coresAllocated += selectedJob.requestedCPUs;
                nodeList.at(selectedNodeID).memoryAllocated += selectedJob.requestedMemory;
                selectedJob.jobStatus = RUNNING;
                selectedJob.nodeID = selectedNodeID;
                runningJobs.push_back(selectedJob);
                // print("Running job ", selectedJob.jobNum, " with a submit time of: ", selectedJob.submitTime, " seconds on node: ", selectedNodeID, "\n");
                // print("Running job ", selectedJob.jobNum, " with a start time of: ", selectedJob.startTime, " seconds\n");
                // print("Running job ", selectedJob.jobNum, " with a requested job runtime of: ", selectedJob.requestedRunTime, " seconds\n");
                outputfile << "Running job " << selectedJob.jobNum << " with a submit time of: " << selectedJob.submitTime << " seconds on node: " << selectedNodeID << "\n";
                outputfile << "Running job " << selectedJob.jobNum << " with a start time of: " << selectedJob.startTime << " seconds\n";
                outputfile << "Running job " << selectedJob.jobNum << " with a requested job runtime of: " << selectedJob.requestedRunTime << " seconds\n";

                // Calculate the core count actually being used, in addition to the requested number:
                int numCPUsInUse = 0;
                unsigned long memoryInUse = 0;
                for (std::vector<Job>::iterator runningJob = runningJobs.begin(); runningJob != runningJobs.end(); ++runningJob)
                {
                    Job currRunningJob = (*runningJob);
                    numCPUsInUse += currRunningJob.usedCPUs;
                    memoryInUse += currRunningJob.usedMemory;
                }

                // Sum up and obtain the totals for our metrics:
                easyMetrics.totalCPUsUsed += numCPUsInUse;
                easyMetrics.maxCPUsUsed = (numCPUsInUse > easyMetrics.maxCPUsUsed) ? numCPUsInUse : easyMetrics.maxCPUsUsed;
                easyMetrics.totalMemoryUsed += memoryInUse;
                easyMetrics.maxMemoryUsed = (memoryInUse > easyMetrics.maxMemoryUsed) ? memoryInUse : easyMetrics.maxMemoryUsed;
            }
            else
            {
                (*waitingJob).waitTime += 1;
                // If no other job was waiting to start at shadowtime, now we are.
                if (reservedJobNum == NO_JOB_RESERVING)
                {
                    reservedJobNum = (*waitingJob).jobNum;
                }
                // Add the rejected job to the waiting list:
                waitingList.push_back(*waitingJob);
            }
        }
        // Recreate the queue with only the processes that are still waiting:
        jobQueue = waitingList;
        // std::cout << "Current time for the EASY algorithm: " << currentTime << std::endl;
        // Increment to the next second: (step-increase in time)
        currentTime++;
        simIteration++;
    }
    // Use finalJobList to calculate metrics.

    outputfile.close();
    return easyMetrics;
}