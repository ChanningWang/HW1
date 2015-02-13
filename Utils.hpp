#include <iostream>
#include <thread>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <time.h>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>
#include "Logging.hpp"


using namespace std;


class Utils{

public:
	static time_t getTime_t(string str){
		std::vector<string> strs;
		//char * time_details;
		boost::split(strs,str,boost::is_any_of("."));
//		for(string s:strs){
//			cout<<s<<endl;
//		}
		//strcpy(time_details, ((string)strs[0]).c_str());
		const char *time_details=((string)strs[0]).c_str();
		struct tm tm;
		strptime(time_details, "%Y%m%d:%H:%M:%S", &tm);
		time_t t = mktime(&tm);

		return t;

	}

	static int cmp(string str1, string str2)
	{
		time_t t1=getTime_t(str1);
		time_t t2=getTime_t(str2);
        long diff=difftime(t1,t2);

		if(diff==0){
		   std::vector<string> strs;
		   boost::split(strs,str1,boost::is_any_of("."));
           string microsec1=strs[1];
           boost::split(strs,str2,boost::is_any_of("."));
           string microsec2=strs[1];
           diff= microsec1.compare(microsec2);
		}
		if(diff>0){
			return 1;
		}else if(diff<0){
			return -1;
		}

		return 0;

	}

	static bool compareTime(string str1, string str2)
		{
			int i=str1.compare(str2);
			if(i>=0){
				return false;
			}else{
				return true;
			}

			return 0;

		}


	long long static getFileSize(string filename){
		ifstream file(filename, ios::binary | ios::ate);
		return file.tellg()/1024;
	}


	static double diffclock(clock_t clock1, clock_t clock2) {
		double diffticks = clock1 - clock2;
		double diffms = (diffticks * 10) / CLOCKS_PER_SEC;
		return diffms;
	}





};




namespace UnitType{
enum class UNIT { GB, MB, KB};
}

