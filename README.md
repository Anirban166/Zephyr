<p align = "center">
<img width = "95%" height = "auto" src = "Images/Finalized logo (reworked for greater width).png">
</p>

---
### General Information
A batch scheduler written in C++ that simulates various pragmatic scheduling algorithms for compute clusters and supercomputers, keeping track of parallel job schedules and compute-node usage (with jobs in the standard workload format provided as input, apart from configurable nodes).

---
### Classes and Fields 
<details> <summary> Job </summary> 
Includes appropriate parameters for a particular job:
  
| Field | Representation |
|:--- | :--- |
| jobNum | Unique identifier for a job |
| nodeID | Node at which the job is running (multiple jobs can have the same ID) |
| stretch | The slowdown, given by the ratio in between a job’s wait time plus actual run time and just the latter |
| waitTime | Difference between start and submit times for a job |
| stopTime | Sum of the start true run times for a job |
| jobStatus | Current state of the job (default is wait mode) |
| startTime | Time the job actually starts running on CPU(s) |
| usedCPUs | Number of cores actually used by a job |
| submitTime | Time the job was submitted by the user |
| usedMemory | Memory actually used by a job |
| trueRunTime | Duration that represents the actual run time for a job (generated statically when creating a job to simulate the amount of time actually needed) |
| requestedCPUs | Requested number of CPU cores |
| turnAroundTime | The difference in between the completion time of the job and the submit time |
| requestedMemory | Memory requested by a job (in GiB) |
| requestedRunTime | User requested (on job creation) job run time |

Fields are set with reference to the standard parallel workload format: https://www.cs.huji.ac.il/labs/parallel/workload/swf.html 
</details>  
  
<details> <summary> Node </summary>   
The parameters here are primarily subjected towards the resources related to a node:
  
| Field | Representation |
|:--- | :--- |
| nodeID | Unique identifier for a node |
| coreCount | The total amount of cores available in the node |
| coresAllocated | Cores used by a job (constrained by above parameters) |
| memoryAmount | The total memory available in the node (in GiB) |
| memoryAllocated | Memory used by a job (constrained by above parameters) |

</details> 
  
<details> <summary> Metrics </summary>   
The variables here are tied to the measures that are ordinally collected to comprehensively portray information for all runs of the scheduler:
  
| Field | Representation |
|:--- | :--- |
| algorithm | The scheduling algorithm that was run (based on user specification) |
| averageWait | The average waiting time for a job |
| longestWait | The longest waiting time reported by a job |
| maxStretch | The maximum stretch observed by a job |
| totalStretch | The total stretch (sum) across all jobs |
| totalWaitSum | Sum of the waiting times across all jobs |
| totalJobsRun | The count of how many jobs were actually run (used to compute the average metrics, as this tends to be the divisor) |
| maxCPUsUsed | The maximum number of CPU cores that have been used simultaneously at a point of time |
| totalCPUsUsed | The total number of CPU cores that have been actually used |
| averageStretch | The average stretch for a job |
| maxMemoryUsed | The maximum amount of memory used at a particular instance of time |
| totalMemoryUsed | The total memory used across all jobs |
| averageCPUsUsed | Number of CPU cores used simultaneously (calculated from the utilization at the time each new job starts) on average |
| totalCPUsRequired | CPU core count that has been requested in total |
| maxTurnAroundTime | The maximum slowdown as experienced by a job |
| totalturnAroundTime | The total slowdown across all jobs |
| averageMemoryUsed | The average amount of memory used at an instance of time |
| averageturnAroundTime | The average slowdown for a job |

</details> 
  
---  
### Algorithms
- Priority-based schemes
    - [Shortest Job First](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/SJF.cpp)
    - [First Come First Serve](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/FCFS.cpp)
- Backfilling strategies    
    - [Conservative Backfilling](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/CBF.cpp)
    - [Extensible Argonne Scheduling System](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/EASY.cpp)

---
### Notes

- **Simulated Time** <br>
For keeping track of the timings, `time_t` variables are used with an alias of ’timestamp’, which indicative of the name itself, represents a particular time point. The time since the UNIX epoch is obtained in seconds and used as the reference time point to compute all the time metrics thereafter. For instance, the submit times (time when the user submits a job to be scheduled) of the jobs would be a few seconds after this timestamp, and the start times (when the job actually runs) would be a few seconds post that. The former is an example of a user-specified time value that is considered in the input jobs (or more specifically, as arguments to the Job class
constructor), and the latter (start time) is an example of a time point that is dynamically ascertained via the scheduler.
- **Algorithms** <br>
One thing that is done in common for each algorithm is the maintainence of a queue for jobs that are ready to be run (for instance, when they satisfy the resource constraints of the nodes). With each push into this queue, the job that is under consideration gets removed from the job list (and attains the running state), and post-completion of its simulated computation, it then gets removed from the queue and the resources it took from a node are reset. The queue is recreated with processes that are only waiting, with no wastage of memory for the buffers used. Worth mentioning that the jobs are sorted based on appropriate parameters (for instance, sorted by the requested run times for SJF, and sorted by the submit times for FCFS) as well.
    - **FCFS** <br>
     Based on submission times, the jobs here are sorted first, and since a [reverse iteration](https://github.com/Anirban166/Zephyr/blob/4eebdc08a3e9f3c9a69fd5706fa00b5dda18ca86/Source/batchScheduler.h#L157) strategy is being followed, they are sorted in a descending manner before plugging them one by one onto the job queue. As is the natural issue with FCFS, processes with less execution time that have later arrival times (or submit times in this case) will have to wait to be scheduled in order, which is not the optimal approach, but in accordance with the nature of this algorithm.
    - **SJF** <br> 
    Here, the queued jobs are sorted based on the user-provided runtimes and then the shortest among them is selected to run first from the list. This causes the issue with large jobs being staled, as they are never selected to run if a stream of small jobs keeps arriving in continuation.
    - **CBF** <br>
    Under conservative backfilling, each and every newly arriving job is given a reservation at the earliest time, such that it does not violate any previously existing guarantees. These start time guarantees are given in arrival order, and the existing reservations act as ceilings in the schedule that prevent later arriving jobs from backfilling easily/inappropriately. Problem here is that long jobs find it harder to backfill with this scheme, since the longer a job is, the more difficult it becomes for it to get a reservation ahead of previously arrived jobs.
    - **EASY** <br>
     For this one, the time at which the first job in the queue starts execution had to be kept track of, whilst maintaining only one reservation (as opposed to one for each job in CBF) for the job in that queue. Note that the concept of 'reservation' here is the same as in CBF above, i.e. a job may be backfilled if and only if it does not delay any other job ahead of it in the queue. Due to the relaxation of the constraint on reservations (having only one at a time), EASY makes it easier to backfill, although there is a chance for the jobs other than the first one to get delayed. Starvation is something that one doesn’t face here, since the delay of the first job is bound by the runtime of the current jobs, although jobs other than the first one can be delayed indefinitely.
- **Project Name and Logo** <br> 
I coined the term for this project to be 'Zephyr' simply out of fanciness, although Flagstaff's weather, Monsoon's login node (more specifically 'wind', the one that I ssh into) and the greek god of the west wind added to the approval of nomenclature. As for my logo, the 'Y' in the word represents the pokemon [Yveltal](https://photos.google.com/share/AF1QipM3GaFPsE2aYDFht4DLknoYV5-2p_nqHYVbd6QobtfeWPGtGPpsQd-MHJ5q0XFQBQ/photo/AF1QipNCIJvG-RncM4ePu5p1vjq3FD2SC5bYR2Ka_X-S?key=cGlORDZzUnp0VGpMdVRVblBCMzRWYlRFby1QVEJ3), which I think is cool :)