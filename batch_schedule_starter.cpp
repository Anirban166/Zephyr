#include "batch_schedule_starter.h"
// Compile and execute using:
// g++ batch_schedule_starter.cpp -o batch_schedule_starter && ./batch_schedule_starter 
/**
   Args: Number of jobs, CPUs available, Memory Available, Nodes available,
   Scheduling algorithm, Verbose
 **/ 
int main(int argc, char* argv[]) 
{
    if(argc != 2)
    {
        std::cout << "Please enter the following arguments:\n";  
        std::cout << "The number of jobs to schedule.\n";  
        exit(-1); 
    }    
    std::cout << "The arguments you entered are:\n";
    for(int i = 0; i < argc; ++i) 
        std::cout << "Argument " << i << ": " << argv[i] << "\n";  

    int jobCount = atoi(argv[1]);
    int nodeCount = 3;
    int coresPerNode = 8;
    //100 GiB per node, i.e. 102400 MiB
    int memoryPerNode = 102400;
    // Generate jobs
    std::vector<Job> jobList = buildPresetJobs();
    // Choose algorithm to run

}

std::vector<Job> buildPresetJobs()
{   
    std::vector<Job> jobList;
    //auto ct = steady_clock::now(); // ::time_point // std::chrono::seconds -> timestamp
   // steady_clock currTime = duration_cast<std::chrono::seconds>(steady_clock::now());
   time_point<system_clock, seconds> start = time_point_cast<seconds>(system_clock::now());
   std::time_t now_c = std::chrono::system_clock::to_time_t(start);
   // Job firstJob = Job(1, currTime, currTime + offset)
   //  std::cout << start_duration;
   

  std::cout << now_c << "\n";

   return jobList;
}
/*
std::vector<Job> buildRandomizedJobs(int jobCount, std::vector<Job> jobList)
{
    

    
  //  double random = lowLimit + (rand() % higherLimit - 1)
    for(int i = 0; i < jobCount; i++)  
    {

        Job currentJob = Job(jobCount, startTime, waitTime, requestedRunTime, trueRunTime, stopTime,
                        //    requestedCPUs, requestedMemory, usedMemory, jobStatus, userId, groupId, precedingJobId, thinkTimeFromPreceding);
        jobList.push_back(currentJob);

    }
} */


