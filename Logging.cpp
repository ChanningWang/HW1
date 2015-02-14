#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
class InitLogging{

public:

	static log4cpp::Category&  getRoot(){
		std::string initFileName = "log4cpp.properties";
				log4cpp::PropertyConfigurator::configure(initFileName);
				return log4cpp::Category::getRoot();
	}


};
