#include <string.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "Record.cpp"

#include <future>
#include <iostream>
#include <chrono>
#include <queue>
#include <vector>
#include <iostream>     // std::cout
#include <algorithm>


using namespace std;
using namespace boost::algorithm;


class OutlierDetecter{

public:
	std::vector<Record> rvector;
	string targetDate;
	float median=0;
	float lowerQtile=0;
    float upperQtile=0;
    float interQtile_range=0;
    float upperInnerFence=0;
    float lowerInnerFence=0;
    float upperOuterFence=0;
    float lowerOuterFence=0;

	int size=0;
	log4cpp::Category& root=InitLogging::getRoot();


	//vector<float> returnV;
	//vector<Record> signalL;
	//vector<Record> noiseL;

	//float

	OutlierDetecter(vector<Record>& vectoring, string targetDate){
       rvector=vectoring;
       this->targetDate=targetDate;
       init();
	}

	~OutlierDetecter(){
		 root.shutdown();
	}

	void init() {
		std::sort(rvector.begin(), rvector.end(), Record::sortByPrice);
		size = rvector.size();

		this->median = getMedian();
		this->lowerQtile = getLowerQuartile();
		this->upperQtile = getUpperQuartile();
		this->interQtile_range = upperQtile - lowerQtile;
		this->upperInnerFence = upperQtile + 1.5 * interQtile_range;
		this->lowerInnerFence = lowerQtile - 1.5 * interQtile_range;
		this->upperOuterFence = upperQtile + 3.0 * interQtile_range;
		this->lowerOuterFence = lowerQtile - 3.0 * interQtile_range;

		std::ostringstream os;
        os<<endl<<"Median="<<median<<endl<<"InterQuantile_Range="<<interQtile_range<<endl
        		<<"upperQtile="<<upperQtile<<endl<<"lowerQtile="<<lowerQtile<<endl
        		<<"upperInnerFence="<<upperInnerFence<<endl<<"lowerInnerFence=" <<lowerInnerFence<<endl
        		<<"upperOuterFence="<<upperOuterFence<<endl<<"lowerOuterFence="<<lowerOuterFence<<endl;
        root.debug(os.str());
//		root.debug("Median=" , boost::lexical_cast<std::string>(median).c_str());
//		root.debug("upperQtile=" , boost::lexical_cast<std::string>(upperQtile).c_str());
//		root.debug("lowerQtile=" , boost::lexical_cast<std::string>(lowerQtile).c_str());
//		root.debug("upperInnerFence=" , boost::lexical_cast<std::string>(upperInnerFence).c_str());
//		root.debug("lowerInnerFence=" , boost::lexical_cast<std::string>(lowerInnerFence).c_str());
//		root.debug("upperOuterFence=" , boost::lexical_cast<std::string>(upperOuterFence).c_str());
//		root.debug("lowerOuterFence=" , boost::lexical_cast<std::string>(lowerOuterFence).c_str());


	}



	float getMedian(){
		if(size%2==0){
			return (((Record)(rvector.at(size/2))).price+((Record)(rvector.at(size/2-1))).price)/2.0;
		}else{
            return ((Record)(rvector.at(size/2))).price;
		}
	}

	float getLowerQuartile(){
		if(size%4==0){
					return (((Record)rvector[size/4]).price+((Record)rvector[size/4-1]).price)/2.0;
				}else{
		            return ((Record)rvector[size/4]).price;
				}
	}

	float getUpperQuartile(){
			if(size*3%4==0){
						return (((Record)rvector[size*3/4]).price+((Record)rvector[size*3/4-1]).price)/2.0;
					}else{
			            return ((Record)rvector[size*3/4]).price;
					}
		}

	// This methods used for scrub and initialize return vector
	void omitAndOutput(string signalFile, string noiseFile){
        remove(signalFile.c_str());
        remove(noiseFile.c_str());
		std::fstream fs_signal(signalFile, ios::out | ios::app | ios::binary);
		std::fstream fs_noise(noiseFile, ios::out | ios::app | ios::binary);
		Record r;

		float pre_price=0;
		float cur_price=0;

		//std::sort(rvector.begin(), rvector.end(), Record::sortByTime);

		for(int i=0;i<size;i++){
			r=(Record)rvector[i];
			if(r.price<this->lowerInnerFence || r.price>this->upperInnerFence){
				r.isNoise=true;
			}else if(r.timestamp.find(targetDate) == std::string::npos){
				r.isNoise=true;
			}
			if(!r.isNoise){
//				cur_price=r.price;
//				if(pre_price!=0){
//                   returnV.push_back(cur_price/pre_price-1);
//				}
//				pre_price=cur_price;
				fs_signal<<r;
			}else{
				fs_noise<<r;
			}
		}

		fs_signal.close();
		fs_noise.close();
	}







};
