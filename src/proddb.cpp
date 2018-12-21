

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
	i210.rowid = -1;
	i217.rowid = -1;
}

RBDE5RData::~RBDE5RData()
{
}

void RBDE5RData::report(std::ostream &os)
{
	char mbstr[100];
	if(i210.rowid != -1)
	{
		std::strftime(mbstr, sizeof(mbstr), "%F %R", std::localtime(&i210.timestamp));
		os << fmt::format("i210: '{0}'  allocated at  {1}\n", i210.addr, mbstr);
	}
	else
		os << "i210: not allocated\n";
	if(i217.rowid != -1)
	{
		std::strftime(mbstr, sizeof(mbstr), "%F %R", std::localtime(&i217.timestamp));
		os << fmt::format("i217: '{0}'  allocated at  {1}\n", i217.addr, mbstr);
	}
	else
		os << "i217: not allocated\n";
	os << std::endl;
}


const std::string &RBDE5RData::if_number(int ni)
{
	if((ni == 0) && (i210.rowid != -1))
		return i210.addr;
	else if((ni == 1) && (i217.rowid != -1))
		return i217.addr;
	else
	{
		SPDLOG_LOGGER_CRITICAL(my_logger, "Error in if_number: ni={0}, i210={1}, i217={2}", ni, i210.rowid, i217.rowid);
		throw "Error in if_number";
	}
}

void RBDE5RData::push_addr(int rowid, const std::string &addr, long long d)
{
	if((i210.rowid == -1) || (i210.rowid == rowid))
	{
		i210.rowid = rowid;
		i210.addr = addr;
		i210.timestamp = d;
	}
	else if(i210.rowid < rowid)
	{
		i217.rowid = rowid;
		i217.addr = addr;
		i217.timestamp = d;
	}
	else // if(i210.rowid > rowid)
	{
		i217 = i210;

		i210.rowid = rowid;
		i210.addr = addr;
		i210.timestamp = d;
	}		
}






ProductDb::ProductDb()
{
}

ProductDb::~ProductDb()
{
}


void ProductDb::connect(const std::string &dbfn)
{
	dbFileName = dbfn;
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



int ProductDb::findId(const std::string &str)
{
	sqlite::database db(dbFileName);

	int id;
	try
	{
		db << "select id from devices where serial=?" << str >> id;
	}
	catch(const sqlite::errors::no_rows &e)
	{
		id = -1;
	}

	SPDLOG_LOGGER_DEBUG(my_logger, "ProductDb::findId  id = {}", id);

	return id;
}

int ProductDb::newId(int type, const std::string &str)
{
	sqlite::database db(dbFileName);

	int id;
	try
	{
		db << "insert into devices(type, serial) values(?,?)" << type << str;
		id = db.last_insert_rowid();
	}
	catch (sqlite::sqlite_exception &e)
	{
		std::cerr  << e.get_code() << ": " << e.what() << " during \""
			<< e.get_sql() << "\"" << std::endl;
		id = -1;
	}

	SPDLOG_LOGGER_DEBUG(my_logger, "ProductDb::newId  id = {}", id);

	return id;
}

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
			>> [&vres](int id, std::string addr, std::time_t date)
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
					<< std::time(nullptr)
					<< s;

			vres.clear();
			db << "select rowid, addr, date from mac_addr where device=?" << id
				>> [&vres](int id, std::string addr, std::time_t date)
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
	try
	{
		sqlite::database db(dbFileName);
		db << "select id, type, aliase from dev_types"
			>> [&os](int id, std::string type, std::string aliase)
				{ os << id << "\t" << type << "\t" << aliase << "\n"; };
		os << std::endl;
	}
	catch (sqlite::sqlite_exception &e)
	{
		std::cerr  << e.get_code() << ": " << e.what() << " during "
			<< e.get_sql() << std::endl;
	}
}


void ProductDb::freeProd(int id)
{
	SPDLOG_LOGGER_TRACE(my_logger, "{}", __PRETTY_FUNCTION__);
	SPDLOG_LOGGER_DEBUG(my_logger, "   id = {}", id);
	try
	{
		sqlite::database db(dbFileName);
		// Освободить MAC-адреса
		db << "update mac_addr set device = NULL where device = ?;" << id;
		// Удалить информацию об устройстве
		db << "delete from devices where id = ?;" << id;
	}
	catch (sqlite::sqlite_exception &e)
	{
		std::cerr  << e.get_code() << ": " << e.what() << " during \""
			<< e.get_sql() << "\"" << std::endl;
	}
}


std::shared_ptr<ProductDb> ProductDb::create()
{
	return std::shared_ptr<ProductDb>(new ProductDb());
}



} // namespace rikor
