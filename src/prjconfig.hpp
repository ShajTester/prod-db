#pragma once


#include <string>
#include <memory>
// #include "rapidjson/rapidjson.h"
// #include "rapidjson/document.h"  


namespace rikor
{


class prjConfig
{

	std::string confFileName;
	std::string dbFileName;
	int ProdTypeId;

	prjConfig();
	prjConfig(std::string fn);

public:

	static std::shared_ptr<prjConfig> create();

	~prjConfig();

	void setFileName(const std::string &fn);
	void setDbFileName(const std::string &fn);
	const std::string &getDbFileName() const;
	void setProdType(const std::string &tn);
	void setProdType(int tp);
	void save();
};


} // namespace rikor