#ifndef __DB_CONNMANAGER_H
#define __DB_CONNMANAGER_H

#include <iostream>
#include <memory>
#include "DBConn.h"

class DBConn;

class DBConnManager
{
public:
	DBConnManager();
	virtual ~DBConnManager();

	bool Init(std::size_t size, std::string& dataSource);
	std::shared_ptr<DBConn>& GetConnection();

private:
	DBConnManager(DBConnManager& rhs);	
	const DBConnManager& operator=(DBConnManager& rhs);

private:
	SQLHENV m_henv;	//定义环境句柄
	std::string m_dataSource;
	std::vector<std::shared_ptr<DBConn>> m_dbConnList;
};


#endif