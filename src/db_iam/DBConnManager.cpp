#include "DBConnManager.h"
#include <sqlext.h>
#include <sql.h>

bool DBConnManager::Init(std::size_t size, const std::string& dataSource)
{
	SQLHENV henv;	//定义环境句柄
	SQLHDBC hdbc;   //定义数据连接句柄
	SQLRETURN ret;
	
	if(MIN_DBCONN_NUM < size || size > MAX_DBCONN_NUM)
	{
		return false;
	}

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);   //分柄环境句柄henv
	if(!SQL_SUCCEEDED(ret))
	{
		std::cout << "SQLAllocHandle henv failed." << std::endl;
		return false;
	}

	ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, SQL_IS_INTEGER); //设置环境属性,ODBC版本号3.x 
	if(!SQL_SUCCEEDED(ret))
	{
		std::cout << "SQLSetEnvAttr failed." << std::endl;
		return false;
	}

	for(int i = 0; i < size; i++)
	{
		std::shared_ptr<DBConn> dbConn(new std::norow(DBConn));
		if(dbConn->Init(dataSource))
		{
			std::cout << "" << std::endl;
			continue;
		}
		m_dbConnList.push_back(dbConn);
	}
	return true;
}

std::shared_ptr<DBConn>& DBConnManager::GetConnection()
{
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
	}

	return m_dbConnList[counter];
}
