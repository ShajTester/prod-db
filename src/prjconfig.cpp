


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


prjConfig::prjConfig(std::string fn): filename(fn)
{
	// Read whole file into a buffer
	FILE* fp = fopen(filename.c_str(), "r");
	fseek(fp, 0, SEEK_END);
	size_t filesize = (size_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buffer = (char*)malloc(filesize + 1);
	size_t readLength = fread(buffer, 1, filesize, fp);
	buffer[readLength] = '\0';
	fclose(fp);

	// d.Parse(buffer);
	if (d.Parse(buffer).HasParseError()) {
	    // fprintf(stderr, "\nError(offset %u): %s\n", 
	    //     (unsigned)d.GetErrorOffset(),
	    //     GetParseError_En(d.GetParseError()));
		SPDLOG_LOGGER_ERROR(my_logger, "Error offset {0}: error number {1}", 
			(unsigned)d.GetErrorOffset(),
			d.GetParseError());
	}
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





} // namespace rikor
