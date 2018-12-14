#pragma once

#include <memory>
#include <sqlite3.h>


namespace rikor
{

class ProductData
{
public:
	ProductData();
	~ProductData();
	
	void report(std::ostream &os);
};



class ProductDb
{

	static int callback(void *NotUsed, int argc, char **argv, char **azColName);
	// Отсюда
	// http://qaru.site/questions/2421964/c-smart-pointers-confusion/6275171#6275171
	struct sqlite3_deleter 
	{
		void operator()(sqlite3* p) const
		{
			sqlite3_close(p);
		}
	};

	using unique_sqlite3 = std::unique_ptr<sqlite3, sqlite3_deleter>;

	unique_sqlite3 db;

	ProductDb();

public:

	static std::shared_ptr<ProductDb> create();
	~ProductDb();

	void connect(const std::string &dbfn);
	void createTable();
	int productId(const std::string &str);
	std::shared_ptr<ProductData> productData(int id);

};

} // namespace rikor