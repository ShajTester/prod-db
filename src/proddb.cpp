

#include <iostream>
#include <memory>
#include <cstdio>

#include "sqlite3.h"

#include "log.h"
#include "proddb.hpp"


namespace rikor
{


ProductData::ProductData()
{
}

ProductData::~ProductData()
{
}

void ProductData::report(std::ostream &os)
{
	os << "ID:  xxx\nProduct:  xxx" << std::endl;
}

std::string ProductData::if_number(int ni)
{
	return std::string("0x1x2x3x4x5x");
}


ProductDb::ProductDb(): db(nullptr)
{
}

ProductDb::~ProductDb()
{
	// if(db) sqlite3_close(db);
}


int ProductDb::callback(void *NotUsed, int argc, char **argv, char **azColName) 
{
	int i;
	for(i = 0; i<argc; i++) 
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}



void ProductDb::connect(const std::string &dbfn)
{
	sqlite3 *p;
	int rc;
	// std::string fn {dbfn};

	/* Open database */
	rc = sqlite3_open(dbfn.c_str(), &p);
	db.reset(p);
	
	if( rc ) 
	{
		std::string tstr {sqlite3_errmsg(db.get())};
		fprintf(stderr, "Can't open database: %s\nFile name: %s\n", tstr, dbfn);
		SPDLOG_LOGGER_CRITICAL(my_logger, "Can't open database: {}", tstr);
		SPDLOG_LOGGER_CRITICAL(my_logger, "File name: {}", dbfn);
		return;
	} 
	else 
	{
		dbFileName = dbfn;
		SPDLOG_LOGGER_DEBUG(my_logger, "Opened database successfully {}", dbfn);
	}
	// Проверяем структуру базы
}


static const char *database_init = 
"CREATE TABLE \"devices\" ("
"    \"id\"     INTEGER PRIMARY KEY AUTOINCREMENT,"
"    \"type\"   INTEGER NOT NULL,"
"    \"serial\" TEXT NOT NULL"
");"

"CREATE TABLE \"mac_addr\" ("
"    \"addr\"   TEXT NOT NULL DEFAULT ('001dc3000000'),"
"    \"device\" INTEGER,"
"    \"date\"   TIMESTAMP"
");"
"CREATE UNIQUE INDEX \"addr\" on mac_addr (addr ASC);"
"CREATE INDEX \"dev\" on mac_addr (device ASC);"

"CREATE TABLE dev_types ("
"    \"id\"     INTEGER PRIMARY KEY AUTOINCREMENT,"
"    \"type\"   TEXT NOT NULL,"
"    \"aliase\" TEXT"
");"
"CREATE UNIQUE INDEX \"type\" on dev_types (type ASC);"
"CREATE UNIQUE INDEX \"aliase\" on dev_types (aliase ASC);"

"BEGIN TRANSACTION;"
"insert into dev_types (\"type\", \"aliase\") values ('R-BD-E5R-V4-16.EA_Ver3', 'ver3');"
"insert into dev_types (\"type\", \"aliase\") values ('R-BD-E5R-V4-16.EA_Ver4', 'ver4');"
"COMMIT;";



void ProductDb::createDB()
{
	char *zErrMsg = 0;
	int rc;

	if(!db)
	{
		SPDLOG_LOGGER_CRITICAL(my_logger, "No database connection");
		throw "No database connection";
		return;
	}
	/* Execute SQL statement */
	rc = sqlite3_exec(db.get(), database_init, NULL, 0, &zErrMsg);
	
	if( rc != SQLITE_OK )
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		SPDLOG_LOGGER_ERROR(my_logger, "SQL error: {}", zErrMsg);
		sqlite3_free(zErrMsg);
		throw "Error while create DB";
	} 
	else 
	{
		SPDLOG_LOGGER_DEBUG(my_logger, "Database created successfully at file {}", dbFileName);
	}
}


int ProductDb::productId(const std::string &str)
{
	return 0;
}

std::shared_ptr<ProductData> ProductDb::productData(int id)
{
	return std::shared_ptr<ProductData>(new ProductData());
}


// Отсюда
// https://stackoverflow.com/a/11238683
void ProductDb::printProdList(std::ostream &os)
{
	if(!db)
	{
		SPDLOG_LOGGER_CRITICAL(my_logger, "No database connection");
		throw "No database connection";
		return;
	}

	char sql[] = "select id, type, aliase from dev_types";
	sqlite3_stmt* stmt;

	sqlite3_prepare(db.get(), sql, sizeof(sql), &stmt, NULL);
	bool done = false;
	while(!done)
	{
		switch(sqlite3_step(stmt))
		{
		case SQLITE_ROW:
			os << sqlite3_column_int(stmt, 0)
				<< "\t" << sqlite3_column_text(stmt, 1)
				<< "\t" << sqlite3_column_text(stmt, 2) << "\n";
			break;
		case SQLITE_DONE:
			done = true;
			break;
		default:
			SPDLOG_LOGGER_ERROR(my_logger, "Error while reading table");
			break;
		}
	}
	sqlite3_finalize(stmt);
	os << std::endl;
}


void ProductDb::freeProd(int id)
{
	if(!db)
	{
		SPDLOG_LOGGER_CRITICAL(my_logger, "No database connection");
		throw "No database connection";
		return;
	}

}


std::shared_ptr<ProductDb> ProductDb::create()
{
	return std::shared_ptr<ProductDb>(new ProductDb());
}



} // namespace rikor
