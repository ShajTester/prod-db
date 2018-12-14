


#include <string>
#include <memory>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"  

#include "prjconfig.hpp"
#include "log.h"

namespace rikor
{




prjConfig::prjConfig()
{
}


prjConfig::prjConfig(std::string fn): confFileName(fn), dbFileName("test.sqlite")
{
}


std::shared_ptr<prjConfig> prjConfig::create(const std::string &fn)
{
	return std::shared_ptr<prjConfig>(new prjConfig(fn));
}

prjConfig::~prjConfig()
{
	if(buffer) free(buffer);
	// Note: At this point, d may have dangling pointers pointed to the deallocated buffer.
}

const rapidjson::Document &prjConfig::getDoc()
{
	return d;
}


const std::string &prjConfig::getDBFileName()
{
	return dbFileName;
}



} // namespace rikor
