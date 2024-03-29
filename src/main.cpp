

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "args.hxx"
#include "version.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "log.h"
#include "prjconfig.hpp"
#include "proddb.hpp"


std::shared_ptr<spdlog::logger> my_logger;


// Отсюда
// https://stackoverflow.com/a/12774387
bool is_file_exists(const std::string &name) 
{
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}

std::string GetCurrentWorkingDir(void) 
{
	char buff[FILENAME_MAX];
	getcwd(buff, FILENAME_MAX);
	return std::string(buff);
}



int main(int argc, char const *argv[])
{

	my_logger = spdlog::stdout_color_st("A");
	my_logger->set_level(spdlog::level::debug);
	// my_logger->trace(" -=- Start");

	// Примеры разбора командной строки
	// https://taywee.github.io/args/

	args::ArgumentParser parser("MAC address database", "Rikor IMT 2018.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag ver(parser, "ver", "version ", {'v', "version"});
	args::ValueFlag<std::string> conf(parser, "conf", "Config file name", {'c', "conf"});
	args::ValueFlag<std::string> dbfile(parser, "dbfile", "Database file name", {'d', "dbfile"});
	args::ValueFlag<std::string> product(parser, "product", "Product name", {'p', "prod"});
	args::ValueFlag<int> log_level(parser, "level", "log level", {"log"});
 //    args::Group jsongroup(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
	// args::Flag json(jsongroup, "json", "json in human readable format", {"json"});
	// args::Flag json1(jsongroup, "json1", "json single line format", {"json1"});
	// args::Flag outtext(jsongroup, "text", "plain text format (default)", {"text"});
	args::Group databasegroup(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
	args::Flag initdb(databasegroup, "initdb", "create new database", {"initdatabase"});
	args::Flag prodlist(databasegroup, "prodlist", "print list of product name", {"prodlist"});
	args::ValueFlag<std::string> sn_for_get(databasegroup, "serial", "The serial number of the product for which to obtain the address.", {"get"});
	args::ValueFlag<std::string> sn_for_free(databasegroup, "serial", "Serial number for which to release the address.", {"free"});
	args::ValueFlag<std::string> info_mac(databasegroup, "MAC-addr", "Get info about MAC-address", {"info-mac"});

	args::ValueFlag<int> interface_number(parser, "number", "Interface number for '--get'.", {"ni"});

	try
	{
		parser.ParseCLI(argc, argv);
	}
	catch (args::Help)
	{
		std::cout << parser;
		return 0;
	}
	catch (args::ParseError e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}
	catch (args::ValidationError e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}


	if(log_level)
	{
		int ll = args::get(log_level);
		if((ll >= SPDLOG_LEVEL_TRACE) && (ll <= SPDLOG_LEVEL_OFF))
			my_logger->set_level(static_cast<spdlog::level::level_enum>(ll));
	}


	auto config = rikor::prjConfig::create();

	if(ver)
	{
		std::cout << "MAC address database\nversion " << VER << "\n\n";
		std::cout << "Config file       : '" << config->getFileName() << "'\n";
		std::cout << "Database file     : '" << config->getDbFileName() << "'\n";
		std::cout << "Default board type: '" << config->getProdType() << "'\n" << std::endl;
		return 0;
	}

	if(conf)
	{ // Загружаем конфигурацию из файла
		SPDLOG_LOGGER_DEBUG(my_logger, "main   Config file name: {}", args::get(conf));
		try
		{
			config->setFileName(args::get(conf));
		}
		catch(const std::exception &e)
		{
			SPDLOG_LOGGER_ERROR(my_logger, "{}", e.what());
			SPDLOG_LOGGER_ERROR(my_logger, "Error loading config {}", args::get(conf));
			return 1;
		}
	}


	if(dbfile) config->setDbFileName(args::get(dbfile));

	if(info_mac)
	{
		auto db = rikor::ProductDb::create();
		try
		{
			db->connect(config->getDbFileName());
			int id = db->IdFromMAC(args::get(info_mac));
			if(id == -1)
			{
				std::cout << fmt::format("Address '{}' does not allocated", args::get(info_mac)) << std::endl;
			}
			else
			{
				auto proddata = db->productData(id);
				proddata->report(std::cout);
			}
		}
		catch(const std::exception &e)
		{
			SPDLOG_LOGGER_ERROR(my_logger, "{}", e.what());
			SPDLOG_LOGGER_ERROR(my_logger, "Database file name {}", config->getDbFileName());
			return 1;
		}
		// Сохраняем используемые параметры
		config->save();
		// Release and close all loggers
		spdlog::drop_all();
		return 0;
	}

	if(product) 
	{
		auto db = rikor::ProductDb::create();
		try
		{
			db->connect(config->getDbFileName());
			int prId = db->getProdTypeId(args::get(product));
			config->setProdType(prId);
		}
		catch(const std::exception &e)
		{
			// Не смогли подключиться к базе
			SPDLOG_LOGGER_ERROR(my_logger, "{}", e.what());
			SPDLOG_LOGGER_ERROR(my_logger, "Database file name {}", config->getDbFileName());
			return 1;
		}
	}


	auto db = rikor::ProductDb::create();

	if(initdb)
	{
		if(!is_file_exists(config->getDbFileName()))
		{
			try
			{
				db->connect(config->getDbFileName());
				db->createDB();
			}
			catch(const std::exception &e)
			{
				// Не смогли подключиться к базе
				// std::cerr << e.what() << std::endl;
				SPDLOG_LOGGER_ERROR(my_logger, "{}", e.what());
				SPDLOG_LOGGER_ERROR(my_logger, "Database file name {}", config->getDbFileName());
				return 1;
			}
		}
		else
		{
			// Существующий файл перезаписывать новой базой не нужно
			SPDLOG_LOGGER_ERROR(my_logger, "File {} already exists. Please enter a different name.", config->getDbFileName());
			return 1;
		}
	}
	else
	{ // Не initdb
		try
		{
			db->connect(config->getDbFileName());

			if(prodlist)
			{
				db->printProdList(std::cout);
			}
			else if(sn_for_get)
			{
				auto id = db->findId(args::get(sn_for_get));
				if(id == -1)
				{
					SPDLOG_LOGGER_INFO(my_logger, "Create new device");
					if(db->checkProdType(config->getProdType()))
						id = db->newId(config->getProdType(), args::get(sn_for_get));
					else
						throw std::runtime_error("Invalid ProdType");
				}

				auto proddata = db->productData(id);
				if(interface_number)
				{
					int ni = args::get(interface_number);
					std::cout << proddata->if_number(ni) << std::flush;
				}
				else
				{
					proddata->report(std::cout);
				}
			}
			else if(sn_for_free)
			{
				auto id = db->findId(args::get(sn_for_free));
				if(id != -1) db->freeProd(id);
				else SPDLOG_LOGGER_ERROR(my_logger, "Serial number '{}' not found", args::get(sn_for_free));
			}
		}
		catch(const std::exception &e)
		{
			// Не смогли подключиться к базе
			SPDLOG_LOGGER_ERROR(my_logger, "{}", e.what());
			SPDLOG_LOGGER_ERROR(my_logger, "Database file name {}", config->getDbFileName());
			return 1;
		}
	}


	// Сохраняем используемые параметры
	config->save();
	// Release and close all loggers
	spdlog::drop_all();
	return 0;
}

