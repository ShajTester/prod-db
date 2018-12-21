#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "sqlite_modern_cpp.h"

//
// What is a good OO C++ wrapper for sqlite [closed]
// https://stackoverflow.com/questions/120295/what-is-a-good-oo-c-wrapper-for-sqlite


namespace rikor
{


struct mac_table_row
{
	int rowid;
	std::string addr;
	int devid;
	std::time_t timestamp;
};



class ProductData
{
public:
	virtual void report(std::ostream &os) = 0;
	virtual const std::string &if_number(int ni) = 0;
	virtual void push_addr(int rowid, const std::string &addr, long long d) = 0;
};


class RBDE5RData : public ProductData
{
	mac_table_row i210;
	mac_table_row i217;

public:	
	RBDE5RData();
	~RBDE5RData();

	void report(std::ostream &os) override;
	const std::string &if_number(int ni) override;
	void push_addr(int rowid, const std::string &addr, long long d) override;
};


class ProductDb
{

	static constexpr unsigned long long start_mac_address = 0x1dc3000000;
	static constexpr unsigned long long max_mac_address = 0x1dc3ffffff;

	std::string dbFileName;

	ProductDb();
	void fill_mac_addr_table(sqlite::connection_type con);

public:

	static std::shared_ptr<ProductDb> create();
	~ProductDb();

	void connect(const std::string &dbfn);
	void createDB();
	int findId(const std::string &str);
	int newId(int type, const std::string &str);
	std::shared_ptr<ProductData> productData(int id);
	void printProdList(std::ostream &os);
	void freeProd(int id);

};

} // namespace rikor

