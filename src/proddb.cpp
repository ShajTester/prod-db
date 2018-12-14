

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
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db.get()));
		return;
	} 
	else 
	{
		fprintf(stdout, "Opened database successfully\n");
	}
	// Проверяем структуру базы
}

void ProductDb::createTable()
{
	char *zErrMsg = 0;
	int rc;
	char *sql;

   /* Create SQL statement */
	sql = "CREATE TABLE COMPANY("  \
	"ID INT PRIMARY KEY     NOT NULL," \
	"NAME           TEXT    NOT NULL," \
	"AGE            INT     NOT NULL," \
	"ADDRESS        CHAR(50)," \
	"SALARY         REAL );";

   /* Execute SQL statement */
	rc = sqlite3_exec(db.get(), sql, ProductDb::callback, 0, &zErrMsg);
	
	if( rc != SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		fprintf(stdout, "Table created successfully\n");
	}
	sqlite3_close(db.get());

}

int ProductDb::productId(const std::string &str)
{
	return 0;
}

std::shared_ptr<ProductData> ProductDb::productData(int id)
{
	return std::shared_ptr<ProductData>(new ProductData());
}


std::shared_ptr<ProductDb> ProductDb::create()
{
	return std::shared_ptr<ProductDb>(new ProductDb());
}



} // namespace rikor
