


#include <string>
#include <memory>

#include "prjconfig.hpp"
#include "log.h"

namespace rikor
{




prjConfig::prjConfig()
{
	dbFileName = "../prod.sqlite3";
	// { // Используем конфигурацию по умолчанию
	// 	configFileName = "/etc/dscan/dscan.conf";
	// 	if(!is_file_exists(configFileName))
	// 	{
	// 		configFileName = GetCurrentWorkingDir();
	// 		configFileName.append("/dscan.conf");
	// 		if(!is_file_exists(configFileName))
	// 		{
	// 			std::cerr << fmt::format("\nNot found config file: {}\n", configFileName) << std::endl;
	// 			return 1;
	// 		}
	// 	}
	// }
}

prjConfig::~prjConfig()
{
}


void prjConfig::setFileName(const std::string &fn)
{
}


void prjConfig::setDbFileName(const std::string &fn)
{
	dbFileName = fn;
}


const std::string &prjConfig::getDbFileName() const
{
	return dbFileName;
}


void prjConfig::setProdType(const std::string &tn)
{
	// Ищем в базе Id
}


void prjConfig::setProdType(int tp)
{
	// Проверяем, что Id присутствует в базе
}


void prjConfig::save()
{
}

std::shared_ptr<prjConfig> prjConfig::create()
{
	return std::shared_ptr<prjConfig>(new prjConfig());
}



} // namespace rikor
