


#include <string>
#include <memory>
#include <fstream>
#include <pwd.h>

#include "prjconfig.hpp"
#include "log.h"

namespace rikor
{




prjConfig::prjConfig()
{
	// Определение домашней директории отсюда:
	// https://ubuntuforums.org/showthread.php?t=2098007&s=ff400108c6ec001310acb3c43c354ce9&p=12421050#post12421050
	passwd* pw = getpwuid(getuid());
    std::string path(pw->pw_dir);
	confFileName = path + "/.config/prod-db.conf";

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


const std::string &prjConfig::getFileName() const
{
	return confFileName;
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
	if(cf.is_open())
	{
		cf >> dbFileName >> ProdTypeId;
		cf.close();
	}
	else
		SPDLOG_LOGGER_ERROR(my_logger, "File '{0}' does not open. Error: {1}", confFileName, errno);

	SPDLOG_LOGGER_TRACE(my_logger, "{}", __PRETTY_FUNCTION__);
	SPDLOG_LOGGER_DEBUG(my_logger, "dbFileName is '{}'", dbFileName);
	SPDLOG_LOGGER_DEBUG(my_logger, "ProdTypeId = {}", ProdTypeId);
}


void prjConfig::save()
{
	SPDLOG_LOGGER_TRACE(my_logger, "{}", __PRETTY_FUNCTION__);
	SPDLOG_LOGGER_DEBUG(my_logger, "dbFileName is '{}'", dbFileName);
	SPDLOG_LOGGER_DEBUG(my_logger, "ProdTypeId = {}", ProdTypeId);

	passwd* pw = getpwuid(getuid());
    std::string fn {pw->pw_dir};
	fn += "/.config/prod-db.conf";
	
	std::ofstream cf;
	cf.open(fn);
	if(cf.is_open())
	{
		cf << dbFileName << "\n" << ProdTypeId << std::endl;
		cf.close();
	}
	else
		SPDLOG_LOGGER_ERROR(my_logger, "File '{0}' does not open. Error: {1}", fn, errno);
}

std::shared_ptr<prjConfig> prjConfig::create()
{
	return std::shared_ptr<prjConfig>(new prjConfig());
}



} // namespace rikor
