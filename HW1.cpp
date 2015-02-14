#include <iostream>
#include <thread>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include "mpi.h"


#include "Normality.cpp"


using namespace std;



int main(int argc, char *argv[]) {
//

	log4cpp::Category& root = InitLogging::getRoot();

	if (argc == 2) {
		if (strcmp(argv[1], "SCRUB") == 0) {
			int rank=0;
			int size=0;
				 MPI_Init (&argc, &argv); /* starts MPI */
				 MPI_Comm_rank (MPI_COMM_WORLD, &rank); /* get current process id */
				 MPI_Comm_size (MPI_COMM_WORLD, &size); /* get number of processes */
			int first_i = (rank - 1) / 26;
			int second_i = (rank - 1) % 26;
			char first_c = first_i + 'a';
			char second_c = second_i + 'a';
			std::ostringstream os;
			os << "bulk" << first_c << second_c;
			string sourceFile = os.str();

			//string sourceFile = "bulkaa";
			string noiseFile = "noise." + sourceFile;
			string signalFile = "signal." + sourceFile;
			Scrubber scrubber(sourceFile, signalFile, noiseFile);
			scrubber.loadIntoMemory();
			scrubber.scrub();

			 //MPI_Finalize();
		}
		if (strcmp(argv[1], "NORMAL") == 0) {
			//Normality norm(scrubber.returnV, 10, 0.1);
			//vector<float> v;
			Normality norm("price.txt", 10, 0.1);
			norm.getBasicStats();
			norm.chiSquareTest();

		}
	}else{
		root.error("[Wrong] need to input Task (SCRUB/NORMAL) as first param");
		return 1;
	}


 root.shutdown();



return 0;
}
