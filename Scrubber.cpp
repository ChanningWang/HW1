#include <string.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "OutlierDetecter.cpp"

#include <condition_variable>
#include <mutex>
#include <future>
#include <iostream>
#include <chrono>
#include <queue>
#include <map>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

using namespace std;
using namespace boost::algorithm;
using namespace UnitType;


class Scrubber{

public:
	 string raw_file="data.txt";
	 //string ordered_file;
	 string signal_file;
	 string noise_file;
//	 int id;

//	 vector<Record> signal_vector;
//	 vector<Record> noise_vector;
	 vector<Record> rvector;
    map<string, long long> dateMap;
    string targetDate;
	 vector<float> returnV;
	 log4cpp::Category & root=InitLogging::getRoot();

	    Scrubber(string raw_file, string signalFile, string noiseFile){
	    	this->raw_file=raw_file;
	    	this->signal_file=signalFile;
	    	this->noise_file=noiseFile;
	    	//this->ordered_file=raw_file+".ordered";
	    	//id=i;

//	    	std::stringstream sstm;
//	    	sstm << "signal.txt." << id;
//	    	signal_file = sstm.str();
//
//	    	std::stringstream sstm1;
//	        sstm1 << "noise.txt." << id;
//	        noise_file = sstm1.str();
	    	//this->bulk_item=10000;

	    }

	    ~Scrubber(){
	    	root.shutdown();
	    }

	   void loadIntoMemory() {
		string line;

		std::ifstream fileStream(raw_file);
		std::vector<string> strs;
		Record r;
        cout<<"Handling File: "<<raw_file<<endl;
		clock_t begin=clock();
		while (getline(fileStream, line, '\n')) {
				trim(line);
				boost::split(strs, line, boost::is_any_of(","));
				r = Record(strs[0], strs[1], strs[2]);
				if(r.price<0 || r.units<=0){
					//noise_vector.push_back(r);
					r.isNoise=true;
				}
				addToDateMap(r.timestamp);
				rvector.push_back(r);
				//cout <<counter<<"-"<<line<<endl;
			}
		clock_t end=clock();
		std::ostringstream os;
		os<<"Load file into Memory and parse takes : "<<Utils::diffclock(end,begin)<<"ms ; Memory Usage: "<<rvector.size()/25000.0<<"mb";
		root.info(os.str());
			//loadIntoFile(signal_vector, signal_file);
			//loadIntoFile(noise_vector,noise_file);

	}

    void scrub(){
    	clock_t begin=clock();
    	findTargetDate();
    	OutlierDetecter detecter(rvector,targetDate);
    	detecter.omitAndOutput(signal_file,noise_file);
//    	returnV=detecter.returnV;
    	clock_t end=clock();
    	std::ostringstream os1;
    	os1<<"Scrub into signal.txt and noise.txt takes : "<<Utils::diffclock(end,begin)<<"ms ; Memory Usage: "<<rvector.size()/25000.0<<"mb";
    	root.info(os1.str());
    }

    void addToDateMap(string dateStr){
    	std::vector<string> strs;
    	boost::split(strs, dateStr, boost::is_any_of(":"));
    	string key=strs[0];
    	map<string,long long>::iterator it = dateMap.find(key);
    	if(it!=dateMap.end()){
    		it->second=it->second+1;
    	}else{
            dateMap[key]=1;
    	}
    }

    void findTargetDate(){
    	long long count=0;
    	for (std::map<string,long long>::iterator it=dateMap.begin(); it!=dateMap.end(); ++it){
    		if(it->second>count){
    			count=it->second;
    			targetDate=it->first;
    		}
    	}
    }



};
