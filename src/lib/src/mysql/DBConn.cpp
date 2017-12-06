#include "DBConn.h"
#include <sstream>

DBConn::DBConn(SQLHENV &henv, std::string& dataSource)
{
	m_hdbc = NULL;
	m_henv = henv;
	m_dataSource = dataSource;
}

DBConn::~DBConn()
{
}

bool DBConn::Release()
{
	SQLRETURN ret = SQLDisconnect(m_hdbc);
	if(!SQL_SUCCEEDED(ret))
	{
		std::cout << "SQLDisconnect failed, ret=" << ret << std::endl;
		return false;
	}

	ret = SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
	if(!SQL_SUCCEEDED(ret))
	{
		std::cout << "SQLFreeHandle failed, ret=" << ret << std::endl;
		return false;
	}

	return true;
}

bool DBConn::Init()
{
	SQLRETURN ret;
	SQLCHAR connStrOut[256] = {0};
	SQLSMALLINT connStrOutLen;
	ret = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);
	if(!SQL_SUCCEEDED(ret))
	{
		std::cout << "SQLAllocHandle failed, ret=" << ret << std::endl;
		return false;
	}

	//TODO: UID从配置文件中读取
	//TODO: PWD从加密文件中读取
	std::string connStr = m_dataSource + ";UID=root;PWD=";

	ret = SQLDriverConnect(m_hdbc, NULL, (SQLCHAR*)(const_cast<char*>(connStr.c_str())),
		SQL_NTS, connStrOut, sizeof(connStrOut), &connStrOutLen, SQL_DRIVER_COMPLETE);
	if(!SQL_SUCCEEDED(ret))	
	{
		std::cout << "SQLDriverConnect failed, ret=" << ret << std::endl;
		SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
		return false;
	}

	std::cout << "Connect database success !!!" << std::endl;

	return true;
}

std::set<std::string> DBConn::m_restartSignals(initRestartSignals());

std::set<std::string> DBConn::initRestartSignals()
{
	std::set<std::string> signals;	
	signals.insert("08003");	
	signals.insert("HY091");	

	return signals;
}

CHECK_RC_E DBConn::IntQuery(std::size_t requestID, std::string command, std::string param, uint64_t* intPtr)
{
	std::size_t size = command.length() + 1;
	char* sql = new (std::nothrow) char[size];
	CHECK_NULL_RETURN(sql);
	memset(sql, '\0', size);
	strncpy(sql, command.c_str(), size);
	sql[size] = '\0';
	CHECK_RC_E rc = IntQuery(requestID, m_hdbc, (SQLCHAR*)sql, param, intPtr);
	delete[] sql;
	sql = NULL;
	return rc;
}

CHECK_RC_E DBConn::IntQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::string& param, uint64_t* intPtr)
{
	HSTMT hsmt;
	SQLRETURN ret;
	const char* operName = "";

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hsmt);
	if(param.size() > 0)
	{
		operName = "SQLPrepare";
		ret = SQLPrepare(hsmt, command, SQL_NTS);
		if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
		{
			std::cout << "SQLPrepare failed, ret=" << ret << std::endl;
			return DB_ERROR;
		}

		operName = "SQLBindParameter";
		BIND_STRING_PARAM(hsmt, param)
		if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
		{
			std::cout << "SQLBindParameter failed, ret=" << ret << std::endl;
			return DB_ERROR;
		}

		operName = "SQLExecute";
		ret = SQLExecute(hsmt);
	}
	else
	{
		operName = "SQLExecDirect";
		ret = SQLExecDirect(hsmt, command, SQL_NTS);
	}

	if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
	{
		std::cout << "Execute SQL command failed, ret=" << ret << std::endl;
		return DB_ERROR;
	}

	operName = "SQLBindCol";
	ret = SQLBindCol(hsmt, 1, SQL_C_UBIGINT, (SQLPOINTER)intPtr, 8, NULL);
	if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
	{
		std::cout << "SQLBindCol failed, ret=" << ret << std::endl;
		return DB_ERROR;
	}

	operName = "SQLFetch";	
	ret = SQLFetch(hsmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
	if(SQL_SUCCEEDED(ret))
	{
		if(ret == SQL_SUCCESS_WITH_INFO)
		{
			printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, hdbc, operName);
		}
		return DONE;
	}
	else if(SQL_NO_DATA == ret)
	{
		return NAME_NOT_FOUND;
	}

	bool isReconnDB = false;
	ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, operName, isReconnDB);
	if(isReconnDB)
	{
		ReconnectDB(requestID, m_hdbc); 
	}

	std::cout << "Return Error:" << ret << std::endl;
	return DB_ERROR;
}

CHECK_RC_E DBConn::IntQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, uint64_t* intPtr)
{
	std::size_t size = command.length() + 1;
	char* sql = new (std::nothrow) char[size];
	CHECK_NULL_RETURN(sql);	memset(sql, '\0', size);
	strncpy(sql, command.c_str(), size);
	sql[size] = '\0';
	CHECK_RC_E rc = generalIntQuery(requestID, m_hdbc, (SQLCHAR*)sql, params, intPtr);
	delete[] sql;
	sql = NULL;
	return rc;
}

CHECK_RC_E DBConn::IntQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint64_t* intPtr)
{
	return generalIntQuery(requestID, m_hdbc, command, params, intPtr);
}

CHECK_RC_E DBConn::generalIntQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint64_t* intPtr)
{
	HSTMT hsmt;
	SQLRETURN ret;
	const char* operName = "";

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hsmt);
	if(params.size() > 0)
	{
		operName = "SQLPrepare";
		ret = SQLPrepare(hsmt, command, SQL_NTS);
		if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
		{
			std::cout << "SQLPrepare failed, ret=" << ret << std::endl;
			return DB_ERROR;
		}

		operName = "SQLBindParameter";
		ret = bindParams(requestID, hdbc, hsmt, params, operName);
		if(DONE != ret)
		{
			std::cout << "SQLBindParameter failed, ret=" << ret << std::endl;
			return DB_ERROR;
		}

		operName = "SQLExecute";
		ret = SQLExecute(hsmt);
	}
	else
	{
		operName = "SQLExecDirect";
		ret = SQLExecDirect(hsmt, command, SQL_NTS);
	}

	if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
	{
		std::cout << "Execute SQL command failed, ret=" << ret << std::endl;
		return DB_ERROR;
	}

	operName = "SQLBindCol";
	ret = SQLBindCol(hsmt, 1, SQL_C_UBIGINT, (SQLPOINTER)intPtr, 8, NULL);
	if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))	
	{
		std::cout << "SQLBindCol failed, ret=" << ret << std::endl;
		return DB_ERROR;
	}

	operName = "SQLFetch";
	ret = SQLFetch(hsmt);

	SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
	if(SQL_SUCCEEDED(ret))
	{
		if(ret == SQL_SUCCESS_WITH_INFO)
		{
			printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, hdbc, operName);
		}
		return DONE;
	}
	else if(SQL_NO_DATA == ret)
	{
		return NAME_NOT_FOUND;
	}

	bool isReconnDB = false;
	ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, operName, isReconnDB);
	if(isReconnDB)
	{
		ReconnectDB(requestID, m_hdbc);	
	}

	std::cout << "Return Error:" << ret << std::endl;
	return DB_ERROR;
}

CHECK_RC_E DBConn::StringQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, std::string& strRef)
{
	std::size_t size = command.length() + 1;
	char* sql = new (std::nothrow) char[size];
	CHECK_NULL_RETURN(sql);	memset(sql, '\0', size);
	strncpy(sql, command.c_str(), size);
	sql[size] = '\0';	
	CHECK_RC_E rc = generalStringQuery(requestID, m_hdbc, (SQLCHAR*)sql, params, strRef);
	delete[] sql;
	sql = NULL;	
	return rc;
}

CHECK_RC_E DBConn::StringQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::string& strRef)
{
	return generalStringQuery(requestID, m_hdbc, command, params, strRef);
}

CHECK_RC_E DBConn::generalStringQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::string& strRef)
{	
	HSTMT hsmt;
	SQLRETURN ret;
	const char* operName = "";
	char buffer[1024] = {0};
	SQLLEN lenAttr;

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hsmt);
	if(params.size() > 0)
	{
		operName = "SQLPrepare";
		ret = SQLPrepare(hsmt, command, SQL_NTS);
		if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))	
		{
			std::cout << "SQLPrepare failed, ret=" << ret << std::endl;
			return DB_ERROR;		
		}

		operName = "SQLBindParameter";
		ret = bindParams(requestID, hdbc, hsmt, params, operName);
		if(DONE != ret)
		{
			std::cout << "SQLBindParameter failed, ret=" << ret << std::endl;
			return DB_ERROR;
		}

		operName = "SQLExecute";
		ret = SQLExecute(hsmt);	
	}
	else
	{
		operName = "SQLExecDirect";
		ret = SQLExecDirect(hsmt, command, SQL_NTS);
	}

	if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
	{
		std::cout << "Execute SQL command failed, ret=" << ret << std::endl;
		return DB_ERROR;
	}

	operName = "SQLBindCol";
	ret = SQLBindCol(hsmt, 1, SQL_C_CHAR, (SQLPOINTER)buffer, sizeof(buffer), &lenAttr);
	if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))	
	{
		std::cout << "SQLBindCol failed, ret=" << ret << std::endl;	
		return DB_ERROR;
	}

	operName = "SQLFetch";	
	ret = SQLFetch(hsmt);	
	SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
	if(SQL_SUCCEEDED(ret))	
	{
		if(ret == SQL_SUCCESS_WITH_INFO)
		{
			printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, hdbc, operName);
		}

		strRef = buffer;
		return DONE;
	}
	else if(SQL_NO_DATA == ret)
	{
		return NAME_NOT_FOUND;	
	}

	bool isReconnDB = false;
	ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, operName, isReconnDB);
	if(isReconnDB)
	{
		ReconnectDB(requestID, m_hdbc);	
	}

	std::cout << "xxxReturn Error:" << ret << std::endl;
	return DB_ERROR;
}

CHECK_RC_E DBConn::dbCommand(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect)
{
	std::size_t size = command.length() + 1;
	char *sql = new (std::nothrow) char[size];
	CHECK_NULL_RETURN(sql);
	memset(sql, '\0', size);
	strncpy(sql, command.c_str(), size);
	sql[size] = '\0';
	std::cout << "SQL command:" << sql << std::endl;
	CHECK_RC_E rc = generalDbCommand(requestID, m_hdbc, (SQLCHAR*)sql, params, numRowAffect);
	delete[] sql;
	sql = NULL;	
	return rc;
}

CHECK_RC_E DBConn::dbCommand(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect)
{
	return generalDbCommand(requestID, m_hdbc, command, params, numRowAffect);
}

CHECK_RC_E DBConn::generalDbCommand(std::size_t requestID,  SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect)
{
	HSTMT hsmt;	
	SQLRETURN ret;
	const char* operName = "";

	SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hsmt);
	if(params.size() > 0)
	{
		operName = "SQLPrepare";
		ret = SQLPrepare(hsmt, command, SQL_NTS);
		if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
		{
			std::cout << "SQLPrepare failed, ret=" <<  ret << std::endl;
			return DB_ERROR;
		}

		operName = "SQLBindParameter";
		ret = bindParams(requestID, hdbc, hsmt, params, operName);
		if(DONE != ret)
		{
			std::cout << "SQLBindParameter failed, ret=" << ret << std::endl;
			return DB_ERROR;
		}

		operName = "SQLExecute";
		ret = SQLExecute(hsmt);
	}
	else	
	{
		operName = "SQLExecDirect";	
		ret = SQLExecDirect(hsmt, command, SQL_NTS);	
	}

	if(numRowAffect)	
	{
		SQLLEN numAffected;	
		ret = SQLRowCount(hsmt, &numAffected);
		if(SQL_SUCCESS != ret)
		{
			std::cout << "failed to get the number of rows affect, 0 as the number of rows affect." << std::endl;
			*numRowAffect = 0;
		}

		*numRowAffect = numAffected;
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hsmt); 

	if(SQL_SUCCEEDED(ret))	
	{
		if(ret == SQL_SUCCESS_WITH_INFO)
		{
			printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, hdbc, operName);
		}
	}
	else
	{
		bool isReconnDB = false;	
		ExtractError(requestID, SQL_HANDLE_DBC, hdbc, operName, isReconnDB);
		if(isReconnDB)
		{
			ReconnectDB(requestID, hdbc);
		}
		return DB_ERROR;
	}

	std::cout << "Return Error:" << ret << std::endl;
	return DONE;	
}

CHECK_RC_E DBConn::checkReturnRCResponseKill(std::size_t requestID, SQLHDBC& hdbc, SQLHDBC& hsmt, SQLRETURN ret, const char* operName)
{	
	if(SQL_SUCCEEDED(ret))
	{
		if(SQL_SUCCESS_WITH_INFO == ret)
		{
			printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, hdbc, operName);
		}
		return DONE;
	}
	else
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, hdbc, operName, isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, hdbc);
		}
		return DB_ERROR;
	}
}

CHECK_RC_E DBConn::checkConnectionRC(std::size_t requestID, SQLHDBC& hdbc, SQLHDBC& hsmt, SQLRETURN ret, const char* operName)
{
	if(SQL_SUCCEEDED(ret))
	{
		if(SQL_SUCCESS_WITH_INFO == ret)
		{
			printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, hdbc, operName);
		}
		return DONE;
	}
	else
	{
		return DB_ERROR;
	}

	return DONE;
}

void DBConn::ExtractError(std::size_t requestID, SQLSMALLINT type, SQLHDBC& handle, const char* operName, bool& isReconnDB)
{
	RETCODE ret;
	SQLCHAR state[6] = {0};	
	SQLINTEGER native;	
	SQLCHAR msg[100];	
	SQLSMALLINT i, msgLen;

	std::cout << "The ODBC Driver throw unexcepted information when running " << operName << std::endl;

	i = 1;
	do	
	{
		ret = SQLGetDiagRec(type, handle, i, state, &native, msg, sizeof(msg), &msgLen);
		if(ret == SQL_SUCCESS)
		{
			std::cout << "ret=" << ret << ", sql state=" << state << ", native error=" << native << ", msg=" << msg << std::endl;
			if(m_restartSignals.end() != m_restartSignals.find((char*)state))
			{
				isReconnDB = true;		
			}
		}
		else if(ret != SQL_NO_DATA)	
		{
			isReconnDB = true;		
			std::cout << "ret=" << ret << std::endl;	
		}

		std::cout << "ExtractError ret=" << ret << std::endl;
		i++;	
	}	while(SQL_NO_DATA != ret);
}

bool DBConn::ReconnectDB(std::size_t requestID, SQLHDBC& hdbc)
{
	SQLRETURN ret;
	SQLCHAR connStrOut[256] = {0};
	SQLSMALLINT connStrOutLen;	
	SQLHDBC tmpHdbc;

	ret = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &tmpHdbc);
	if(!SQL_SUCCEEDED(ret))	
	{
		std::cout << "SQLAllocHandle connection handle failed." << std::endl;
		return false;	
	}

	//TODO: UID从配置文件中读取
	//TODO: PWD从加密文件中读取
	//TODO: 从配置文件中读取重试次数和间隔时间	

	std::string connStr = m_dataSource + ";UID=root;PWD=";	
	int reconnectTimes = 6;
	int intervalTime = 60; //60s  

	for(int i = 0; i < reconnectTimes; i++)
	{
		ret = SQLDriverConnect(tmpHdbc, NULL, (SQLCHAR*)(const_cast<char*>(connStr.c_str())), 
			SQL_NTS, connStrOut, sizeof(connStrOut), &connStrOutLen, SQL_DRIVER_COMPLETE);
		if(!SQL_SUCCEEDED(ret))		
		{
			std::cout << "SQLDriverConnect failed, ret=" << ret << std::endl;
			sleep(intervalTime);	
			continue;
		}

		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		m_hdbc = tmpHdbc;
		std::cout << "Reconnect database success !!!" << std::endl;	
		return true;
	}

	SQLFreeHandle(SQL_HANDLE_DBC, tmpHdbc); 
	std::cout << "Reconnect database failed !!!" << std::endl; 
	return false;
}

CHECK_RC_E DBConn::bindParams(std::size_t requestID, SQLHDBC& hdbc, HSTMT& hsmt, std::vector<ParamToBind_t>& params, const char* operName)
{
	SQLRETURN ret;	
	SQLUSMALLINT i;	

	for(i = 1; i <= params.size(); i++)
	{
		if(params[i-1].paramType == STRING_PARAM || params[i-1].paramType == PATTERN_PARAM)	
		{	
			std::cout << "bind string param, i=" << i << ", strVal=" << params[i-1].strVal << std::endl;	
			ret = SQLBindParameter(hsmt, i, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, 255, 0, 
				(SQLPOINTER)(const_cast<char*>(params[i-1].strVal.c_str())), params[i-1].strVal.size(), NULL);
		}	
		else if(params[i-1].paramType == INT_PARAM)	
		{			
			ret = SQLBindParameter(hsmt, i, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_BIGINT, 8, 0, 
				(SQLPOINTER)(&params[i-1].intVal), 0, NULL);	
		}	
		else	
		{		
			std::cout << "unkown param type." << std::endl;	
			//return DB_ERROR;		
		}	

		if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, ret, operName))
		{		
			return DB_ERROR;	
		}	
	}	
	return DONE;
}

void DBConn::printSQLGetDiagRec(std::size_t requestID, SQLSMALLINT type, SQLHANDLE& handle, const char* operName)
{	
	RETCODE ret;
	SQLCHAR state[6] = {0};
	SQLINTEGER native;	
	SQLCHAR msg[100];	
	SQLSMALLINT i, msgLen;

	std::cout << "The ODBC Driver throw unexcepted information when running " << operName << std::endl;	

	i = 1;	
	do
	{	
		ret = SQLGetDiagRec(type, handle, i, state, &native, msg, sizeof(msg), &msgLen);
		if(ret == SQL_SUCCESS)	
		{		
			std::cout << "ret=" << ret << ", sql state=" << state << ", native error=" << native << ", msg=" << msg << std::endl;	
		}	
		else if(ret != SQL_NO_DATA)	
		{		
			std::cout << "ret=" << ret << std::endl;	
		}		

		i++;
	}	while(SQL_NO_DATA != ret);
}

CHECK_RC_E DBConn::dbQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, std::vector<QueryResult_t>& result)
{
	std::size_t size = command.length() + 1;
	char* sql = new (std::nothrow) char[size];
	CHECK_NULL_RETURN(sql);	
	memset(sql, '\0', size);
	strncpy(sql, command.c_str(), size);
	sql[size] = '\0';	
	CHECK_RC_E rc = dbQuery(requestID, (SQLCHAR*)sql, params, result);
	delete[] sql;
	sql = NULL;
	return rc;
}

CHECK_RC_E DBConn::dbQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::vector<QueryResult_t>& result)
{
	HSTMT hsmt;	
	SQLRETURN ret;
	const char* operName = "";
	SQLLEN *rLenAttr = NULL;
	std::vector<char *> cString;
	
	SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hsmt);
	if(params.size() > 0)
	{	
		operName = "SQLPrepare";	
		ret = SQLPrepare(hsmt, command, SQL_NTS);	
		if(DONE != checkReturnRCResponseKill(requestID, m_hdbc, hsmt, ret, operName))	
		{		
			std::cout << "SQLPrepare failed, ret=" << ret << std::endl;	
			return DB_ERROR;	
		}	

		operName = "SQLBindParameter";	
		ret = bindParams(requestID, m_hdbc, hsmt, params, operName);	
		if(DONE != ret)	
		{			
			std::cout << "SQLBindParameter failed, ret=" << ret << std::endl;
			return DB_ERROR;	
		}		

		operName = "SQLExecute";
		ret = SQLExecute(hsmt);
	}
	else
	{		
		operName = "SQLExecDirect";		
		ret = SQLExecDirect(hsmt, command, SQL_NTS);
	}	

	if(DONE != checkReturnRCResponseKill(requestID, m_hdbc, hsmt, ret, operName))
	{		
		std::cout << "Execute SQL comand failed, ret=" << ret << std::endl;		
		return DB_ERROR;	
	}

	rLenAttr = new SQLLEN[result.size()];
	if(NULL == rLenAttr)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
		return SYSTEM_OUT_MEM;
	}
	std::cout << "dbQuery:###########################1" << std::endl;

	ret = bindOutput(requestID, m_hdbc,  hsmt,  result, cString, rLenAttr);
	if(DONE != ret)
	{	
		std::cout << "Bind SQL column failed, ret=" << ret << std::endl;
		cleanTmpData(rLenAttr, result, cString);  
		return DB_ERROR;	
	}

	std::cout << "dbQuery:###########################2" << std::endl;
	ret = SQLFetch(hsmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
	std::cout << "dbQuery:###########################3" << std::endl;

	if(SQL_SUCCEEDED(ret))	
	{		
		if(ret == SQL_SUCCESS_WITH_INFO)	
		{		
			printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, m_hdbc, operName);	
		}             
		std::cout << "dbQuery:###########################4" << std::endl;
		int cntNull = processNullData(requestID, result, rLenAttr); 
		if(cntNull == result.size())   
		{        
			cleanTmpData(rLenAttr, result, cString);    
			return NAME_NOT_FOUND;  
		}       
		std::cout << "dbQuery:###########################5" << std::endl;

		processStringData(requestID, result, cString, rLenAttr);     
		std::cout << "dbQuery:###########################6" << std::endl;
		cleanTmpData(rLenAttr, result, cString);
		std::cout << "dbQuery:###########################7" << std::endl;
		return DONE;	
	}
	else if(SQL_NO_DATA == ret)
	{       
		cleanTmpData(rLenAttr, result, cString);		
		return NAME_NOT_FOUND;
	}

	bool isReconnDB = false;
	ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, operName, isReconnDB);
	if(isReconnDB)	
	{
		ReconnectDB(requestID, m_hdbc);	
	}	

	cleanTmpData(rLenAttr, result, cString);
	std::cout << "Return Error:" << ret << std::endl;	
	return DB_ERROR;
}

CHECK_RC_E DBConn::bindOutput(std::size_t requestID, SQLHDBC& hdbc,  HSTMT& hsmt,  std::vector<QueryResult_t>& result, std::vector<char*>& cString, SQLLEN* rLenAttr)
{
	SQLUSMALLINT i;
	SQLRETURN rc;
	CHECK_RC_E ret;
	SQLLEN* attr;
	const char* operName = "SQLBindCol";
	
	for(i = 1; i < result.size() + 1; i++)
	{    
		attr = rLenAttr + i - 1;     
		if(result[i-1].paramType == STRING_PARAM || result[i-1].paramType == PATTERN_PARAM)  
		{          
			cString.push_back(new char[1024]);
			std::cout << "SQLBindCol i = " << i << std::endl;
			rc = SQLBindCol(hsmt, i, SQL_C_CHAR, cString[cString.size() - 1], 1024, attr);   
		}       
		else if(result[i-1].paramType == INT_PARAM)   
		{           
			rc = SQLBindCol(hsmt, i, SQL_C_UBIGINT, result[i-1].intVal, 8, attr);   
		}       
		else if(result[i-1].paramType == BLOB_PARAM)    
		{      
			cString.push_back(new char[1024]);     
			rc = SQLBindCol(hsmt, i, SQL_C_CHAR, cString[cString.size() - 1], result[i-1].strSize, attr);    
		}     
		else   
		{      
			rc = DB_ERROR;  
		}             

		if(DONE != checkReturnRCResponseKill(requestID, hdbc, hsmt, rc, operName))  
		{         
			return DB_ERROR;  
		}   
	}     
	return DONE;
}

void DBConn::cleanTmpData(SQLLEN* rLenAttr, std::vector<QueryResult_t>&result, std::vector<char*>& cString)
{    
	if(rLenAttr)  
	{      
		delete [] rLenAttr;   
		rLenAttr = NULL;  
	}   

	int strIndex = 0;  
	for(int i = 0; i < result.size(); i++) 
	{       
		if(result[i].paramType != INT_PARAM)   
		{        
			if(cString[strIndex])  
			{             
				delete [] cString[strIndex];   
				cString[strIndex] = NULL;
				strIndex++;
			}          
			if(strIndex == cString.size())  
			{               
				break;   
			}  
		}  
	}
}

int DBConn::processNullData(std::size_t requestID, std::vector<QueryResult_t>& result, SQLLEN* rLenAttr)
{  
	int numNullData;   
	for(int i = 0; i < result.size(); i++) 
	{    
		if(rLenAttr[i] == SQL_NULL_DATA)   
		{          
			if(result[i].paramType == INT_PARAM)  
			{              
				*result[i].intVal = 0;    
			}         
			else    
			{       
				*result[i].strVal = std::string("");
			}          
			numNullData++;    
		}  
	}   
	return numNullData;
}

void DBConn::processStringData(std::size_t requestID, std::vector<QueryResult_t>&result, std::vector<char*>& cString, SQLLEN* rLenAttr)
{
	int strIndex = 0;
	for(int i = 0; i < result.size(); i++)
	{
		if(result[i].paramType == STRING_PARAM || result[i].paramType == PATTERN_PARAM) 
		{
			if(rLenAttr[i] == SQL_NULL_DATA)
			{    
				*result[i].strVal = std::string(""); 
			}         
			else    
			{   
				std::cout << "i = " << i << ", cString:" << cString[strIndex] << std::endl;
				std::cout << "i = " << i << ", rLenAttr:" << rLenAttr[i] << std::endl;
				*result[i].strVal = std::string(cString[strIndex], rLenAttr[i]);
				strIndex++;        
			}     
		}  
	}
}

CHECK_RC_E DBConn::copyRow(std::size_t requestID, 
	int row, 
	FiledRouter& router, 
	std::vector<FlexQueryResult>& buffers,
	std::vector<SQLLEN*>& indicatorArray)
{    
	std::cout << "The size of router is " << router.getSize() << std::endl;
	for(int i = 0; i < router.getSize(); i++)
	{
		std::cout << "i=" << i << ", type=" << router.getFiledType(i) << std::endl;
		if(router.getFiledType(i) == INT_PARAM)
		{
			*router.getFiledIntPtr(i) = buffers[i].intPtr[row];
			std::cout << "i=" << i << ", value=" << *router.getFiledIntPtr(i) << std::endl;
			continue;
		}

		std::cout << "begin to start extract string object ..., indicator=" << indicatorArray.size() << std::endl;
		std::cout << "begin to start extract string object ..., indicator=" << indicatorArray[i][row] << std::endl;
		if(indicatorArray[i][row] == -1)
		{       
			*router.getFiledStrPtr(i) = "";
			std::cout << "i=" << i << ", value=" << *router.getFiledStrPtr(i) << std::endl;
			continue;
		}                

		std::string tmpStr = std::string(buffers[i].strPtr + (row * buffers[i].strSize), indicatorArray[i][row]);
		std::cout << "i=" << i << ", value=" << tmpStr << std::endl;

		*router.getFiledStrPtr(i) = tmpStr;
		std::cout << "i=" << i << ", value=" << *router.getFiledStrPtr(i) << std::endl;
	}

	return DONE;
}

CHECK_RC_E DBConn::bindColums(std::size_t requestID, 
	HSTMT& hsmt, 
	FiledRouter& router, 
	std::vector<FlexQueryResult>& buffers,
	IndicatorAarry& indicatorArray)
{    
	SQLUSMALLINT i;
	SQLRETURN rc;
	const char* operName = "SQLBindCol";
	bool isReconnDB = false;
	
	for(i = 1; i <= router.getSize(); i++)
	{        
		SQLLEN* attr = indicatorArray.addIndicator();
		buffers.push_back(FlexQueryResult());

		buffers[i - 1].paramType = router.getFiledType(i - 1);
		buffers[i - 1].strSize = router.getFiledSize(i - 1);
		if(buffers[i - 1].paramType == INT_PARAM)
		{
			buffers[i - 1].intPtr = new (std::nothrow) uint64_t[FETCH_ROW_NUM];
			if(NULL == buffers[i - 1].intPtr)
			{                
				std::cout << "flexQuery:: memory allocation failed." << std::endl;
				cleanBuffers(buffers);
				return SYSTEM_OUT_MEM;
			}			

			rc = SQLBindCol(hsmt, i, SQL_C_UBIGINT,  buffers[i - 1].intPtr, 8, attr); 
		}        
		else
		{
			//if(buffers[i - 1].strSize * FETCH_ROW_NUM > ULONG_MAX)
			//{
			//	std::cout << "Lost of data converation ...." << std::endl;  
			//}            

			buffers[i - 1].strPtr = new char[buffers[i - 1].strSize * FETCH_ROW_NUM];
			if(NULL == buffers[i - 1].strPtr)
			{
				std::cout << "flexQuery:: memory allocation failed." << std::endl; 
				cleanBuffers(buffers);
				return SYSTEM_OUT_MEM;
			}            

			if(buffers[i - 1].paramType == STRING_PARAM)
			{       
				rc = SQLBindCol(hsmt, i, SQL_C_CHAR, buffers[i - 1].strPtr, buffers[i - 1].strSize, attr); 
			}
			else //BLOB 
			{        
				rc = SQLBindCol(hsmt, i, SQL_C_BINARY, buffers[i - 1].strPtr, buffers[i - 1].strSize, attr);
			}
		}

		if(SQL_SUCCEEDED(rc)) 
		{
			if(SQL_SUCCESS_WITH_INFO == rc) 
			{                
				printSQLGetDiagRec(requestID, SQL_HANDLE_DBC, m_hdbc, operName); 
			}
		}
		else
		{
			SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
			ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, operName, isReconnDB);
			if(isReconnDB)	
			{
				ReconnectDB(requestID, m_hdbc);
			}            
			cleanBuffers(buffers);
			return DB_ERROR;   
		}          
	}

	return DONE;
}

CHECK_RC_E DBConn::flexQueryExecute(std::size_t requestID, HSTMT &hsmt, SQLCHAR* command, std::vector<ParamToBind_t>& params)
{
	const char* operName = "";
	SQLRETURN ret;
	
	if(params.size() > 0)
	{	
		operName = "SQLPrepare";	
		ret = SQLPrepare(hsmt, command, SQL_NTS);	
		if(DONE != checkReturnRCResponseKill(requestID, m_hdbc, hsmt, ret, operName))	
		{		        
			std::cout << "SQLPrepare failed, ret=" << ret << std::endl;	  
			return DB_ERROR;	       
		}	      
		
		operName = "SQLBindParameter";	  
		ret = bindParams(requestID, m_hdbc, hsmt, params, operName);	 
		if(DONE != ret)	       
		{			   
			std::cout << "SQLBindParameter failed, ret=" << ret << std::endl;    
			return DB_ERROR;	        
		}		       

		operName = "SQLExecute"; 
		ret = SQLExecute(hsmt);    
	}     
	else    
	{		
		operName = "SQLExecDirect";		
		ret = SQLExecDirect(hsmt, command, SQL_NTS);  
	}
	
	if(DONE != checkReturnRCResponseKill(requestID, m_hdbc, hsmt, ret, operName))   
	{		       
		std::cout << "Execute SQL comand failed, ret=" << ret << std::endl;		
		return DB_ERROR;	   
	}     

	return DONE;
}

void DBConn::cleanBuffers(std::vector<FlexQueryResult>& buffers)
{
	for(int i = 0; i < buffers.size(); i++)
	{
		if(buffers[i].paramType == INT_PARAM)
		{
			if(buffers[i].intPtr)
			{
				delete[] buffers[i].intPtr;
				buffers[i].intPtr = NULL;
			}
		}
		else
		{
			if(buffers[i].strPtr)
			{
				delete[] buffers[i].strPtr;
				buffers[i].strPtr = NULL;
			}
		}
	}
}

CHECK_RC_E DBConn::StartTranscation(std::size_t requestID)
{
	SQLRETURN rc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_IS_INTEGER);
	if(!SQL_SUCCEEDED(rc))
	{
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "SQLSetConnectAttr", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR; 
	}

	SQLINTEGER autoCommitMode;
	rc = SQLGetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)&autoCommitMode, sizeof(SQLINTEGER), NULL); 
	if(!SQL_SUCCEEDED(rc))
	{
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "SQLGetConnectAttr", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR; 
	}

	if(SQL_AUTOCOMMIT_OFF != autoCommitMode)
	{
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR;
	}

	return DONE;
}

CHECK_RC_E DBConn::EndTranscation(std::size_t requestID)
{
	SQLINTEGER autoCommitMode;
	SQLRETURN rc;
	
	rc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_IS_INTEGER);
	if(!SQL_SUCCEEDED(rc))
	{
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "SQLSetConnectAttr", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR; 
	}

	rc = SQLGetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)&autoCommitMode, sizeof(SQLINTEGER), NULL); 
	if(!SQL_SUCCEEDED(rc))
	{
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "SQLGetConnectAttr", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR; 
	}

	if(SQL_AUTOCOMMIT_ON != autoCommitMode)
	{
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR;
	}

	return DONE;
}

CHECK_RC_E DBConn::RollbackRranscation(std::size_t requestID)
{
	bool isReconnDB = false;
	
	SQLRETURN rc = SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK);
	if(!SQL_SUCCEEDED(rc))
	{
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "SQLEndTran", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR; 
	}

	rc = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_IS_INTEGER);
	if(!SQL_SUCCEEDED(rc))
	{
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, "SQLSetConnectAttr", isReconnDB);
		if(isReconnDB)	
		{
			ReconnectDB(requestID, m_hdbc);
		}
		return DB_ERROR; 
	}
	
	return DONE;
}

IndicatorAarry::~IndicatorAarry()
{
	for(int i = 0; i < indicatorArray.size(); i++) 
	{
		if(indicatorArray[i]) 
		{
			delete[] indicatorArray[i]; 
			indicatorArray[i] = NULL;
		}
	}
};

SQLLEN* IndicatorAarry::addIndicator()
{
	SQLLEN* addr = new SQLLEN[FETCH_ROW_NUM];
	indicatorArray.push_back(addr);
	return addr;
}


