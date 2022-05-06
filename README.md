### General
A batch scheduler written in C++ that simulates various real-world scheduling algorithms for compute clusters, keeping track of parallel job schedules and compute-node usage.

### Classes and fields 

<details> <summary> Job </summary> 
Includes appropriate parameters for a particular job:
  
| Field | Representation |
|:--- | :--- |
| jobNum | Unique identifier for a job |
| nodeID | Node at which the job is running (multiple jobs can have the same ID) |
| submitTime | Time the job was submitted by the user |
| startTime | Time the job actually starts running on CPU(s) |
| waitTime | Difference between start and submit times for a job |
| requestedRunTime | User requested (on job creation) job run time |
| trueRunTime | Duration that represents the actual run time for a job (generated statically when creating a job to simulate the amount of time actually needed) |
| stretch | The slowdown, given by the ratio in between a job’s wait time plus actual run time and just the latter |
| turnAroundTime | The difference in between the completion time of the job and the submit time |
| stopTime | Sum of the start true run times for a job |
| jobStatus | Current state of the job (default is wait mode) |
| requestedCPUs | Requested CPU cores |
| usedCPUs | Cores actually used by a job |
| requestedMemory | Memory requested by a job (in GiB) |
| usedMemory | Memory actually used by a job |

Fields are set with reference to the standard workload format: https://www.cs.huji.ac.il/labs/parallel/workload/swf.html 
  
</details>  
  
<details> <summary> Node </summary>   
The parameters here are primarily subjected towards the resources related to a node:
  
| Field | Representation |
|:--- | :--- |
| nodeID | Unique identifier for a node |
| coreCount | The total amount of cores available in the node |
| memoryAmount | The total memory available in the node (in GiB) |
| coresAllocated | Cores used by a job (constrained by above parameters) |
| memoryAllocated | Memory used by a job (constrained by above parameters) |

</details> 
  
<details> <summary> Metrics </summary>   
The variables here are tied to the measures that are ordinally collected to comprehensively portray information for all runs of the scheduler:
  
| Field | Representation |
|:--- | :--- |
| algorithm | The scheduling algorithm that was run (based on user specification) |
| totalWaitSum | Sum of the waiting times across all jobs |
| averageWait | The average waiting time for a job |
| longestWait | The longest waiting time reported by a job |
| averageturnAroundTime | The average slowdown for a job |
| totalturnAroundTime | The total slowdown across all jobs |
| maxTurnAroundTime | The maximum slowdown as experienced by a job |
| averageStretch | The average stretch for a job |
| totalStretch | The total stretch (sum) across all jobs |
| maxStretch | The maximum stretch observed by a job |
| totalCPUsUsed | The total number of CPU cores that have been actually used |
| totalCPUsRequired | CPU core count that has been requested in total |
| maxCPUsUsed | The maximum number of CPU cores that have been used simultaneously at a point of time |
| totalMemoryUsed | The total memory used across all jobs |
| maxMemoryUsed | The maximum amount of memory used at a particular instance of time |
| averageCPUsUsed | The average number of CPU cores used at one instance of time (calculated from the utilization at the time each new job starts) |

</details> 
  
## Algorithms
- [Shortest Job First](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/SJF.cpp)
- [First Come First Serve](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/FCFS.cpp)
- [Conservative Backfilling](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/CBF.cpp)
- [Extensible Argonne Scheduling System](https://github.com/Anirban166/Zephyr/blob/main/Source/Algorithms/EASY.cpp)