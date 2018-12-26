


#include <string>
#include <memory>
#include <fstream>

#include "prjconfig.hpp"
#include "log.h"

namespace rikor
{




prjConfig::prjConfig()
{
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
	confFileName = "~/.config/prod-db.conf";
	if(is_file_exists(confFileName))
	{
		readConfig();
	}
	else
	{
		confFileName = "/etc/prod-db/prod-db.conf";
		if(is_file_exists(confFileName))
		{
			readConfig();
		}
		else
		{
			dbFileName = "../prod.sqlite3";
			setProdType(2);
		}
	}
}

prjConfig::~prjConfig()
{
}


void prjConfig::setFileName(const std::string &fn)
{
	confFileName = fn;
	readConfig();
}


void prjConfig::setDbFileName(const std::string &fn)
{
	dbFileName = fn;
}


const std::string &prjConfig::getDbFileName() const
{
	return dbFileName;
}


void prjConfig::setProdType(int tp)
{
	ProdTypeId = tp;
}


int prjConfig::getProdType()
{
	return ProdTypeId;
}


void prjConfig::readConfig()
{
	std::ifstream cf;
	cf.open(confFileName);
	cf >> dbFileName >> ProdTypeId;
	cf.close();
	SPDLOG_LOGGER_DEBUG(my_logger, "{}", __PRETTY_FUNCTION__);
	SPDLOG_LOGGER_DEBUG(my_logger, "dbFileName is '{}'", dbFileName);
	SPDLOG_LOGGER_DEBUG(my_logger, "ProdTypeId = {}", ProdTypeId);
}


void prjConfig::save()
{
	SPDLOG_LOGGER_DEBUG(my_logger, "{}", __PRETTY_FUNCTION__);
	SPDLOG_LOGGER_DEBUG(my_logger, "dbFileName is '{}'", dbFileName);
	SPDLOG_LOGGER_DEBUG(my_logger, "ProdTypeId = {}", ProdTypeId);
	// std::string fn {"~/.config/prod-db.conf"};
	std::string fn {"~/prod-db.conf"};
	std::ofstream cf;
	cf.open(fn);
	if(cf.is_open())
	{
		cf << dbFileName << "\n" << ProdTypeId << std::endl;
		cf.close();
	}
	else
		SPDLOG_LOGGER_ERROR(my_logger, "File '{}' does not open", fn);
}

std::shared_ptr<prjConfig> prjConfig::create()
{
	return std::shared_ptr<prjConfig>(new prjConfig());
}



} // namespace rikor
