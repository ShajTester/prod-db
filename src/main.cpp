

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
#include "sqlite3.h"

#include "log.h"
#include "prjconfig.hpp"

std::shared_ptr<spdlog::logger> my_logger;


// Отсюда
// https://stackoverflow.com/a/12774387
inline bool is_file_exists(const std::string &name) 
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

static int callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
	int i;
	for(i = 0; i<argc; i++) 
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}





int main(int argc, char const *argv[])
{

	my_logger = spdlog::stdout_color_st("A");
	my_logger->set_level(spdlog::level::trace);
	// my_logger->trace(" -=- Start");

	// Примеры разбора командной строки
	// https://taywee.github.io/args/

	args::ArgumentParser parser("MAC address database", "Rikor IMT 2018.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag ver(parser, "ver", "version ", {'v', "version"});
	args::ValueFlag<std::string> conf(parser, "conf", "Config file name", {'c', "conf"});
	args::ValueFlag<std::string> dbfile(parser, "dbfile", "Database file name", {'f', "dbfile"});
	args::ValueFlag<std::string> product(parser, "product", "Product name", {'p', "prod"});
 //    args::Group jsongroup(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
	// args::Flag json(jsongroup, "json", "json in human readable format", {"json"});
	// args::Flag json1(jsongroup, "json1", "json single line format", {"json1"});
	// args::Flag outtext(jsongroup, "text", "plain text format (default)", {"text"});
	args::Group databasegroup(parser, "This group is all exclusive:", args::Group::Validators::AtMostOne);
	args::Flag initdb(databasegroup, "initdb", "create new database", {"initdatabase"});
	args::ValueFlag<std::string> sn_for_get(parser, "snget", "The serial number of the product for which to obtain the address.", {"get"});

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


	if(ver)
	{
		std::cout << "MAC address database\nversion " << VER << std::endl;
		return 0;
	}


	std::string configFileName;

	if(conf)
	{ // Загружаем конфигурацию из файла
		SPDLOG_LOGGER_DEBUG(my_logger, "main   Config file name: {}", args::get(conf));
		configFileName = args::get(conf);
		if(!is_file_exists(configFileName))
		{
			std::cerr << "\nNot found config file\n" << std::endl;
			return 1;
		}
	}
	// else
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


	SPDLOG_LOGGER_INFO(my_logger, "Config file name: {}", configFileName);

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;

   /* Open database */
	rc = sqlite3_open("test.db", &db);
	
	if( rc ) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	} else {
		fprintf(stdout, "Opened database successfully\n");
	}

   /* Create SQL statement */
	sql = "CREATE TABLE COMPANY("  \
	"ID INT PRIMARY KEY     NOT NULL," \
	"NAME           TEXT    NOT NULL," \
	"AGE            INT     NOT NULL," \
	"ADDRESS        CHAR(50)," \
	"SALARY         REAL );";

   /* Execute SQL statement */
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db);

	// Release and close all loggers
	spdlog::drop_all();
	return 0;
}

