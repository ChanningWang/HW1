#include <string.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <future>
#include <iostream>
#include <chrono>
#include <queue>
#include <vector>
#include <iostream>     // std::cout
#include <algorithm>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <math.h>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/math/distributions.hpp>
#include <map>
#include <boost/math/distributions/chi_squared.hpp>
#include "Scrubber.cpp"


using namespace boost::accumulators;
using namespace std;
using namespace boost::algorithm;

using boost::math::normal;


class Normality{



public:
	string rawFile;
	vector<float> returnV;
	float E=0;
	float std=0;
	int size=0;
	int freedom=0 ;
	float significant_level=0.1;
	log4cpp::Category & root=InitLogging::getRoot();


	int bin_number;
	vector<float> points;
	map<int,int> binMap;
//	Normality(vector<float>& v, int numberOfBin, float significant_level){
//
//        this->returnV=v;
//        this->bin_number=numberOfBin;
//        this->freedom=numberOfBin-3;
//        this->significant_level=significant_level;
//
//
//	}

	Normality(string file, int numberOfBin, float significant_level){

	       // this->returnV=v;
		    this->rawFile=file;
	        this->bin_number=numberOfBin;
	        this->freedom=numberOfBin-3;
	        this->significant_level=significant_level;
	        loadData();


		}


	~Normality(){
		root.shutdown();
	}


	void loadData() {
		string line;
		std::ifstream fileStream(rawFile);
		float pre_price=0;
		float cur_price=0;

		while (getline(fileStream, line, '\n')) {
			trim(line);
			if(pre_price==0){
				pre_price=boost::lexical_cast<float>(line);
			}else{
				cur_price=boost::lexical_cast<float>(line);
				returnV.push_back((cur_price/pre_price)-1);
				pre_price=cur_price;
			}
		}
	}


	void getBasicStats(){
		accumulator_set<float, stats<tag::mean,tag::variance> > acc;
		  acc=for_each(returnV.begin(), returnV.end(), acc);
          E=mean(acc);
          size=returnV.size();
          std=sqrt((float)variance(acc));

          std::ostringstream os;
          os<<endl<<"---Basic Return Specs---"<<endl<<"Size of Sample="<<size<<endl<<"Mean of Retrun="<<E<<endl<<
        		  "Standard Deviation of Return="<<std;
          root.info(os.str());
//          root.info("---Basic Return Specs---");
//          root.info( "Size of Sample=",size);
//          root.info("Mean of Retrun=",E);
//          root.info("Standard Deviation of Return=", std);

	}

	//get the separate points
	void binStats(){
		boost::math::normal_distribution<> snd(0, 1);
		points=vector<float>(bin_number-1);
		std::ostringstream os;
        for(int i=0;i<bin_number-1;i++){
        	points[i]=E+ std * boost::math::quantile(snd,(double)(i+1)*1.0/bin_number);

        	 os<<endl<<"points["<<i<<"]"<<points[i];
        }
        root.debug(os.str());

	}

	int getBinIndex(float value){
		int index=-1;
		for(int i=0;i<bin_number-1;i++){
			if(value<=points[i]){
				index= i;
				break;
			}
		}
		if(index==-1) index=bin_number-1;
		return index;
	}

	//statistics on number of records in each bin
	void populateBinMap() {
		int bin_index = 0;
		map<int, int>::iterator it;
		for(int i = 0; i < bin_number; i++){
			binMap[i] = 0;
		}
		for (int i = 0; i < size; i++) {
			bin_index = getBinIndex(returnV[i]);
			//cout<<returnV[i]<<"-"<<bin_index<<endl;
			it = binMap.find(bin_index);
			if (it != binMap.end()) {
				it->second = it->second + 1;
			} else {
				binMap[bin_index] = 1;
			}

		}

		std::ostringstream os;
		for (std::map<int,int>::iterator it=binMap.begin(); it!=binMap.end(); ++it){
			os<<endl<<it->first<<" : "<<it->second;
			  //root.debug((char*)it->first,":",(char*)it->second);
		 }
		root.debug(os.str());
	}

	float getChiSquare(){
		vector<float> result(bin_number);
		float chi_value=0;
		int f=0;
		float e=size*1.0/bin_number;
		for(int i=0;i<bin_number;i++){
           f=binMap[i];
           result[i]=(f-e)*(f-e)/e;
           chi_value+=result[i];
		}

		return chi_value;

	}


	bool chiSquareTest(){
		bool isNormal=false;
		boost::math::chi_squared mydist(bin_number-1-2);
		binStats();
		populateBinMap();
		float chi_value=getChiSquare();
		double chi_table=boost::math::quantile(mydist,1.0-significant_level);
		std::ostringstream os;
		os<<endl<<"Chi-Square from sample="<<chi_value;
		os<<endl<<"Chi-Square from Normal Dist="<<chi_table;
//		root.info("Chi-Square from sample=",chi_value);
//		root.info("Chi-Square from Normal Dist=",chi_table);
		root.info(os.str());

        if(chi_value<=chi_table){
           isNormal=true;
        }

        if (isNormal) {
        			root.info("Is this return sample normal distributed? --YES");
        		} else {
        			root.warn("Is this return sample normal distributed? --NO");
        		}

      return isNormal;
	}

};
