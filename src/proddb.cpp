

#include <iostream>
#include <memory>
#include <cstdio>
#include <ctime>
#include <vector>

// #include "sqlite3.h"

#include "sqlite_modern_cpp.h"

#include "log.h"
#include "proddb.hpp"


namespace rikor
{


RBDE5RData::RBDE5RData()
{
}

RBDE5RData::~RBDE5RData()
{
}

void RBDE5RData::report(std::ostream &os)
{
	os << "ID:  xxx\nProduct:  xxx" << std::endl;
}

std::string RBDE5RData::if_number(int ni)
{
	return std::string("0x1x2x3x4x5x");
}

void RBDE5RData::push_addr(int rowid, std::string addr, long long d)
{
}






ProductDb::ProductDb()
{
}

ProductDb::~ProductDb()
{
	// if(db) sqlite3_close(db);
}


// int ProductDb::callback(void *NotUsed, int argc, char **argv, char **azColName) 
// {
// 	int i;
// 	for(i = 0; i<argc; i++) 
// 	{
// 		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
// 	}
// 	printf("\n");
// 	return 0;
// }



void ProductDb::connect(const std::string &dbfn)
{

		dbFileName = dbfn;

	// Здесь проверяем, что файл базы доступен

	// sqlite3 *p;
	// int rc;
	// // std::string fn {dbfn};

	// /* Open database */
	// rc = sqlite3_open(dbfn.c_str(), &p);
	// db.reset(p);
	
	// if( rc ) 
	// {
	// 	std::string tstr {sqlite3_errmsg(db.get())};
	// 	fprintf(stderr, "Can't open database: %s\nFile name: %s\n", tstr.c_str(), dbfn.c_str());
	// 	SPDLOG_LOGGER_CRITICAL(my_logger, "Can't open database: {}", tstr);
	// 	SPDLOG_LOGGER_CRITICAL(my_logger, "File name: {}", dbfn);
	// 	return;
	// } 
	// else 
	// {
	// 	SPDLOG_LOGGER_DEBUG(my_logger, "Opened database successfully {}", dbfn);
	// }
	// Проверяем структуру базы
}


static const char *database_init1 = 
"CREATE TABLE \"devices\" ("
"    \"id\"     INTEGER PRIMARY KEY AUTOINCREMENT,"
"    \"type\"   INTEGER NOT NULL,"
"    \"serial\" TEXT NOT NULL"
");";

static const char *database_init2 = 
"CREATE TABLE \"mac_addr\" ("
"    \"addr\"   TEXT NOT NULL DEFAULT ('001dc3000000'),"
"    \"device\" INTEGER,"
"    \"date\"   INTEGER,"
"    \"userid\" TEXT"
");";
static const char *database_init3 = 
"CREATE UNIQUE INDEX \"addr\" on mac_addr (addr ASC);";
static const char *database_init4 = 
"CREATE INDEX \"dev\" on mac_addr (device ASC);";

static const char *database_init5 = 
"CREATE TABLE dev_types ("
"    \"id\"     INTEGER PRIMARY KEY AUTOINCREMENT,"
"    \"type\"   TEXT NOT NULL,"
"    \"aliase\" TEXT"
");";
static const char *database_init6 = 
"CREATE UNIQUE INDEX \"type\" on dev_types (type ASC);";
static const char *database_init7 = 
"CREATE UNIQUE INDEX \"aliase\" on dev_types (aliase ASC);";

static const char *database_init8 = 
"BEGIN TRANSACTION;";
static const char *database_init9 = 
"insert into dev_types (\"type\", \"aliase\") values ('R-BD-E5R-V4-16.EA_Ver3', 'ver3');";
static const char *database_init10 = 
"insert into dev_types (\"type\", \"aliase\") values ('R-BD-E5R-V4-16.EA_Ver4', 'ver4');";
static const char *database_init11 = 
"COMMIT;";



void ProductDb::createDB()
{
	// char *zErrMsg = 0;
	// int rc;

	// if(!db)
	// {
	// 	SPDLOG_LOGGER_CRITICAL(my_logger, "No database connection");
	// 	throw "No database connection";
	// 	return;
	// }
	// /* Execute SQL statement */
	// rc = sqlite3_exec(db.get(), database_init, NULL, 0, &zErrMsg);
	
	// if( rc != SQLITE_OK )
	// {
	// 	fprintf(stderr, "SQL error: %s\n", zErrMsg);
	// 	SPDLOG_LOGGER_ERROR(my_logger, "SQL error: {}", zErrMsg);
	// 	sqlite3_free(zErrMsg);
	// 	throw "Error while create DB";
	// } 
	// else 
	// {
	// 	SPDLOG_LOGGER_DEBUG(my_logger, "Database created successfully at file {}", dbFileName);
	// }

	// sqlite::sqlite_config config;
	// config.flags = sqlite::OpenFlags::CREATE;

	// sqlite::database db(dbFileName, config);

	SPDLOG_LOGGER_DEBUG(my_logger, "Database file: {}", dbFileName);

	sqlite::database db(dbFileName);

	db << database_init1;
	db << database_init2;
	db << database_init3;
	db << database_init4;
	db << database_init5;
	db << database_init6;
	db << database_init7;
	db << database_init9;
	db << database_init10;

	SPDLOG_LOGGER_DEBUG(my_logger, "Database created successfully at file {}", dbFileName);
}



void ProductDb::fill_mac_addr_table(sqlite::connection_type con)
{
	sqlite::database db(con);
	std::string addrstr;
	long long unsigned int addr;

	SPDLOG_LOGGER_DEBUG(my_logger, "{}", __PRETTY_FUNCTION__);

	try
	{
		db << "select addr from mac_addr where rowid in (select max(rowid) from mac_addr)" >> addrstr;
		int ret = std::sscanf(addrstr.c_str(), "%llx", &addr);
		if(ret != 1) addr = start_mac_address;
		else addr++;
	
		SPDLOG_LOGGER_DEBUG(my_logger, "addrstr: {}", addrstr);
		SPDLOG_LOGGER_DEBUG(my_logger, "   addr: {:012x}", addr);
	
	}
	catch(const std::exception &e)
	{
		addr = start_mac_address;
	}
	try
	{
		db << "begin;";
		for(int i=0; i<100; i++)
		{
			db << "insert into mac_addr (addr) values (?)" << fmt::format("{:012x}", (addr + i));
		}
		db << "commit;";

		SPDLOG_LOGGER_DEBUG(my_logger, "fill_mac_addr_table   ALL DONE");
	}
	catch(const std::exception &e)
	{
		SPDLOG_LOGGER_DEBUG(my_logger, "Error in insert: {}", e.what());
		db << "rollback;";
	}	
}



int ProductDb::productId(const std::string &str)
{
	sqlite::database db(dbFileName);

	int id;
	try
	{
		db << "select id from devices where serial=?" << str >> id;
	}
	catch(const sqlite::errors::no_rows &e)
	{
		db << "insert into devices(type, serial) values(?,?)" << 2 << str;
		id = db.last_insert_rowid();
	}

	SPDLOG_LOGGER_DEBUG(my_logger, "ProductDb::productId  id = {}", id);

	return id;
}


struct mac_table_row
{
	int rowid;
	std::string addr;
	int devid;
	long long int timestamp;
};


std::shared_ptr<ProductData> ProductDb::productData(int id)
{
	sqlite::database db(dbFileName);
	
	int devtype;
	std::shared_ptr<ProductData> retval;

	try
	{
		db << "select type from devices where id=?" << id >> devtype;
	}
	catch(const sqlite::errors::no_rows &e)
	{
		throw "Unknown device id";
	}

	switch(devtype)
	{
	case 1:
	case 2:
		retval = std::make_shared<RBDE5RData>();
		break;
	default:
		throw "Unknown type";
		break;
	}

	SPDLOG_LOGGER_DEBUG(my_logger, "Device type is {}", devtype);

	// Алгоритм получения алресов зависит от типа устройства.
	// Пока устройство одно, реализуем все здесь.
	try
	{
		// Если адреса уже были выделены, просто их читаем.
		std::vector<mac_table_row> vres;
		db << "select rowid, addr, date from mac_addr where device=?" << id
			>> [&vres](int id, std::string addr, long long date)
				{ 
					vres.push_back(mac_table_row{id, addr, 0, date});
				};
		
		SPDLOG_LOGGER_DEBUG(my_logger, "Addresses for device {0} already reserved in rowcnt = {1}", id, vres.size());


		if(vres.size() != 2)
		{
			// Сюда попадаем, если адреса для id небыли выделены.
			// Пытаемся выделить новые адреса
			int rowcnt = 0;
			db << "select count(*) from mac_addr where device is null" >> rowcnt;
			
			SPDLOG_LOGGER_DEBUG(my_logger, "Свободных адресов в базе {}", rowcnt);
			
			if(rowcnt < 2)
			{
				fill_mac_addr_table(db.connection());
			}

			// Выбираем 2 адреса
			std::vector<std::string> sa;
			db << "select addr from mac_addr where device is null limit 2"
				>> [&sa](std::string a){sa.push_back(a);};
			if(sa.size() != 2)
			{
				SPDLOG_LOGGER_CRITICAL(my_logger, "Error in SQL");
				throw "Error in SQL";
			}
			for(const auto &s: sa)
				db << "update mac_addr set device=?, date=? where addr=?" 
					<< id 
					<< std::to_string((long long)std::time(nullptr))
					<< s;

			vres.clear();
			db << "select rowid, addr, date from mac_addr where device=?" << id
				>> [&vres](int id, std::string addr, long long date)
					{ 
						vres.push_back(mac_table_row{id, addr, 0, date});
					};
		}

		// 
		for(const auto &it: vres)
		{
			retval->push_addr(it.rowid, it.addr, it.timestamp);
		}

	}
	catch(const std::exception &e)
	{
		throw "Unable to allocate addresses";
	}

	return retval;
}


// Отсюда
// https://stackoverflow.com/a/11238683
void ProductDb::printProdList(std::ostream &os)
{
	// if(!db)
	// {
	// 	SPDLOG_LOGGER_CRITICAL(my_logger, "No database connection");
	// 	throw "No database connection";
	// 	return;
	// }

	// char sql[] = "select id, type, aliase from dev_types";
	// sqlite3_stmt* stmt;

	// sqlite3_prepare(db.get(), sql, sizeof(sql), &stmt, NULL);
	// bool done = false;
	// while(!done)
	// {
	// 	switch(sqlite3_step(stmt))
	// 	{
	// 	case SQLITE_ROW:
	// 		os << sqlite3_column_int(stmt, 0)
	// 			<< "\t" << sqlite3_column_text(stmt, 1)
	// 			<< "\t" << sqlite3_column_text(stmt, 2) << "\n";
	// 		break;
	// 	case SQLITE_DONE:
	// 		done = true;
	// 		break;
	// 	default:
	// 		SPDLOG_LOGGER_ERROR(my_logger, "Error while reading table");
	// 		break;
	// 	}
	// }
	// sqlite3_finalize(stmt);
	// os << std::endl;
}


void ProductDb::freeProd(int id)
{
	// if(!db)
	// {
	// 	SPDLOG_LOGGER_CRITICAL(my_logger, "No database connection");
	// 	throw "No database connection";
	// 	return;
	// }

}


std::shared_ptr<ProductDb> ProductDb::create()
{
	return std::shared_ptr<ProductDb>(new ProductDb());
}



} // namespace rikor
