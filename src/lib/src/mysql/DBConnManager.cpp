#include "DBConnManager.h"
#include <sqlext.h>
#include <sql.h>

#define MIN_DBCONN_NUM 1
#define MAX_DBCONN_NUM 100

DBConnManager::DBConnManager()
{
	
}

DBConnManager::~DBConnManager()
{
	for(int i = 0; i < m_dbConnList.size(); i++)
	{
		std::shared_ptr<DBConn> conn = m_dbConnList[i];
		if(!conn->Release())
		{
			std::cout << "release db conn manager failed" << std::endl;
		}
	}

	SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
}

bool DBConnManager::Init(std::size_t size, std::string& dataSource)
{
	SQLRETURN ret;
	
	if(MIN_DBCONN_NUM < size || size > MAX_DBCONN_NUM)
	{
		return false;
	}

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);   //分柄环境句柄henv
	if(!SQL_SUCCEEDED(ret))
	{
		std::cout << "SQLAllocHandle henv failed." << std::endl;
		return false;
	}

	ret = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, SQL_IS_INTEGER); //设置环境属性,ODBC版本号3.x 
	if(!SQL_SUCCEEDED(ret))
	{
		std::cout << "SQLSetEnvAttr failed." << std::endl;
		return false;
	}

	m_dataSource = dataSource;
	for(int i = 0; i < size; i++)
	{
		std::shared_ptr<DBConn> dbConn(new(std::nothrow) DBConn(m_henv, m_dataSource));
		if(!dbConn->Init())
		{
			std::cout << "alloc db connection object failed." << std::endl;
			continue;
		}
		m_dbConnList.push_back(dbConn);
	}
	return true;
}

std::shared_ptr<DBConn>& DBConnManager::GetConnection()
{
	//需要加锁
	int size = m_dbConnList.size();
	int NO_INITIAL = size * 2 + 1;
	int counter = NO_INITIAL;
	
	for(int i = 0; i < m_dbConnList.size(); i++)
	{
		if(1 == m_dbConnList[i].use_count())
		{
			counter = i;
			break;
		}
	}

	if(counter == NO_INITIAL)
	{
		//重新分配新的连接对象
		std::shared_ptr<DBConn> dbConn(new(std::nothrow) DBConn(m_henv, m_dataSource));
		if(!dbConn->Init())
		{
			std::cout << "alloc db connection object failed." << std::endl;
			//continue;
		}
		m_dbConnList.push_back(dbConn);
		counter = size;
		size++;
	}
	std::cout << "get conn, counter=" << counter << "/" << size << std::endl;

	return m_dbConnList[counter];
}
