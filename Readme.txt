----------------------
Design Logic
----------------------
1) When we get large sized data file, we first split within unix into small bulks. Each bulk with special id to identify.
2) Then we invoke mpirun to run our scrub process. The rank number of mpi is uniquely mapped to the bulk ID/name. So each process would 
know which file to process. Each process generate its own signal.txt and noise.txt.
The noise condition includes:
     (1) non-positive price or units
     (2) records not in the same date
     (3) prices fall outside the inner-quantile fence
3)Then we concatenate all the signal and noise files into one respectively.
4)Then we sort our signal.txt by timestamp, cut the price list to another file called price.txt.
Then we load price.txt into our NORMAL function part to do basic data analysis and normality check.
The normality check is based on chi-square (10 bins involed)


----------------------
Tool Used
----------------------
c++ 11
log4cpp
boost 
open mpi
unix scripting
git

----------------------
File Description
----------------------

Logging.cpp             --- provided log function with log4cpp lib
Utils.cpp               --- provided common shared useful utilities
Record.cpp              --- basic pojo to model one line of record in the data file
OutlierDetecter.cpp     --- class to provide functions to filter out noise from signal data
Scrubber.cpp            --- wrap up functional class to conduct scrub operation
Normality.cpp           --- class to provide function to analyze return data normality (chi-square)
HW1.cpp                 --- main class for execution. It will receive one program argument "SCRUB" OR "NORMAL"
                            to identify the exact action to take for the specified task.
                           
scrub                   --- scripts to run to invoke HW1
chengbin_makefile       --- hw1 makefile
log4cpp.properties      --- config file for log4cpp appenders


----------------------
Execution Description
----------------------

1)  git clone https://github.com/ChanningWang/HW1.git
2) cd HW1
3)  make -f chengbin_makefile
4) chmod 777 *
5) ./scrub [dataFile]
6)  vi Assignment_A.log   



----------------------
Professor check
----------------------

My project has been loaded and compiled here on penzias:

/scratch/chengbin.wang/Assignment_A/HW1

Just cd to there and run ./scrub [dataFile].

After done, checking log Assignment_A.log


Snippets of log info:

.......
[total] Data Loading and Scrubbing takes 
[total] Combine and generating Singal.txt takes 
[total] Combine and generating noise.txt  takes 
......
---Basic Return Specs---
......
 Is this return sample normal distributed?


