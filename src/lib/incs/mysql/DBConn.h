#ifndef __DBCONN_H
#define __DBCONN_H

#include <sqlext.h>
#include <sql.h>
#include <set>
#include <unistd.h>
#include <string.h>
#include "DBAdapterOpt.h"

using namespace std;

#define CHECK_NULL_RETURN(param)		\
		if(NULL == param)				\
			return SYSTEM_OUT_MEM;		\
			
#define BIND_INT_PARAM(hsmt, param)		\
			ret = SQLBindParameter(hsmt, 1, SQL_PARAM_INPUT, SQL_C_UBIGINT, SQL_BIGINT, 8, 0, (SQLPOINTER)(&param), 0, NULL);
		
#define BIND_STRING_PARAM(hsmt, param)	\
			ret = SQLBindParameter(hsmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_LONGVARCHAR, 255, 0, (SQLPOINTER)(const_cast<char*>(param.c_str())), param.size(), NULL);

enum CHECK_RC_E
{
	DONE,
	SYSTEM_OUT_MEM,
	NAME_NOT_FOUND,
	DB_ERROR	
};

#define FETCH_ROW_NUM	50

class IndicatorAarry
{
public:
	~IndicatorAarry();
	SQLLEN* addIndicator();

	std::vector<SQLLEN*> indicatorArray;
};

class DBConn
{
public:
	DBConn(SQLHENV &henv, std::string& dataSource);
	virtual ~DBConn();
	
	bool Release();
	bool Init();

	CHECK_RC_E dbCommand(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect = NULL);
	CHECK_RC_E dbCommand(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect = NULL);
	
	CHECK_RC_E IntQuery(std::size_t requestID, std::string command, std::string param, uint64_t* intPtr);
	CHECK_RC_E IntQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::string& param, uint64_t* intPtr);
	CHECK_RC_E IntQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, uint64_t* intPtr);
	CHECK_RC_E IntQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint64_t* intPtr);

	CHECK_RC_E StringQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, std::string& strRef);
	CHECK_RC_E StringQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::string& strRef);

	CHECK_RC_E dbQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, std::vector<QueryResult_t>& result);
	CHECK_RC_E dbQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::vector<QueryResult_t>& result);

	template <typename RowStruct>
	CHECK_RC_E flexQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, std::vector<RowStruct>& results);
	template <typename RowStruct>
	CHECK_RC_E flexQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::vector<RowStruct>& results);

	CHECK_RC_E StartTranscation(std::size_t requestID);
	CHECK_RC_E EndTranscation(std::size_t requestID);
	CHECK_RC_E RollbackRranscation(std::size_t requestID);
private:
	CHECK_RC_E generalDbCommand(std::size_t requestID,  SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect);
	void ExtractError(std::size_t requestID, SQLSMALLINT type, SQLHDBC& hdbc, const char* operName, bool& isReconnDB);
	bool ReconnectDB(std::size_t requestID, SQLHDBC& hdbc);
	CHECK_RC_E bindParams(std::size_t requestID, SQLHDBC& hdbc, HSTMT& hsmt, std::vector<ParamToBind_t>& params, const char* operName);
	CHECK_RC_E checkReturnRCResponseKill(std::size_t requestID, SQLHDBC& hdbc, SQLHDBC& hsmt, SQLRETURN ret, const char* operName);
	CHECK_RC_E checkConnectionRC(std::size_t requestID, SQLHDBC& hdbc, SQLHDBC& hsmt, SQLRETURN ret, const char* operName);

	void printSQLGetDiagRec(std::size_t requestID, SQLSMALLINT type, SQLHANDLE& handle, const char* operName);

	CHECK_RC_E generalIntQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint64_t* intPtr);
	CHECK_RC_E generalStringQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::string& strRef);

	CHECK_RC_E bindOutput(std::size_t requestID, SQLHDBC& hdbc, HSTMT& hsmt, std::vector<QueryResult_t>& result, std::vector<char*>& cString, SQLLEN* rLenAttr);
	void cleanTmpData(SQLLEN* rLenAttr, std::vector<QueryResult_t>&result, std::vector<char*>& cString);
	int processNullData(std::size_t requestID, std::vector<QueryResult_t>& result, SQLLEN* rLenAttr);
	void processStringData(std::size_t requestID, std::vector<QueryResult_t>&result, std::vector<char*>& cString, SQLLEN* rLenAttr);
	void cleanBuffers(std::vector<FlexQueryResult>& buffers);

	CHECK_RC_E flexQueryExecute(std::size_t requestID, HSTMT &hsmt, SQLCHAR* command, std::vector<ParamToBind_t>& params);
	CHECK_RC_E bindColums(std::size_t requestID, HSTMT& hsmt, FiledRouter& router, std::vector<FlexQueryResult>& buffers,IndicatorAarry& indicatorArray);
	CHECK_RC_E copyRow(std::size_t requestID, int row, FiledRouter& router, std::vector<FlexQueryResult>& buffers,std::vector<SQLLEN*>& indicatorArray);

	static std::set<std::string> initRestartSignals();
	
private:
	static std::set<std::string> m_restartSignals;
	SQLHENV m_henv; //定义环境句柄
	SQLHDBC m_hdbc;   //定义数据连接句柄
	std::string m_dataSource;
};

template <typename RowStruct>
CHECK_RC_E DBConn::flexQuery(std::size_t requestID, std::string command, std::vector<ParamToBind_t>& params, std::vector<RowStruct>& results)
{
	std::size_t size = command.length() + 1;
	char* sql = new char[size]; 
	CHECK_NULL_RETURN(sql);
	memset(sql, '\0', size); 
	strncpy(sql, command.c_str(), size); 
	sql[size] = '\0';   
	CHECK_RC_E rc = flexQuery(requestID, (SQLCHAR*)sql, params, results); 
	delete[] sql;    
	sql = NULL;  
	return rc;
}

template <typename RowStruct>
CHECK_RC_E DBConn::flexQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::vector<RowStruct>& results)
{
	HSTMT hsmt;
	IndicatorAarry indicators;
	std::vector<FlexQueryResult> buffers;
	RowStruct  rowStruct;    
	SQLUSMALLINT rowStatus[FETCH_ROW_NUM];
	SQLLEN  rowOffset = 0;    
	uint64_t rowCnt = 0, row_cof = 0;
	CHECK_RC_E rc;
	SQLRETURN ret;
	const char* operName = "";
	
	SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hsmt);

	rc = flexQueryExecute(requestID, hsmt, command, params);
	if(DONE != rc)
	{
		return DB_ERROR;
	}
	std::cout << "flexQuery: flexQueryExecute end ..." << std::endl;

	rc = bindColums(requestID, hsmt, rowStruct.getRouter(), buffers, indicators);
	if(DONE != rc)
	{
		return DB_ERROR;
	}
	std::cout << "flexQuery: bindColums end ..." << std::endl;

	SQLSetStmtAttr(hsmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)FETCH_ROW_NUM, SQL_NTS);
	
	do
	{
		operName = "SQLFetchScroll";
		SQLSetStmtAttr(hsmt, SQL_ATTR_ROWS_FETCHED_PTR, (SQLPOINTER)&rowCnt, 0);
		SQLSetStmtAttr(hsmt, SQL_ATTR_ROW_STATUS_PTR, (SQLPOINTER)rowStatus, 0);

		ret = SQLFetchScroll(hsmt, SQL_FETCH_NEXT, rowOffset);
		if(DONE != checkConnectionRC(requestID, m_hdbc, hsmt, ret, operName))
		{
			break;
		}
		std::cout << "flexQuery: SQLFetchScroll end ..." << std::endl;
		
		for(int row = 0; row < rowCnt; row++)
		{
			if((rowStatus[row] != SQL_ROW_SUCCESS) && (rowStatus[row] != SQL_SUCCESS_WITH_INFO))
			{
				break;
			}
			std::cout << "flexQuery: SQLFetchScroll xxx ..." << std::endl;

			results.push_back(RowStruct());
			rc = copyRow(requestID, row, results[row_cof].getRouter(), buffers, indicators.indicatorArray);
			if(DONE != rc)
			{
				std::cout << "copy row failed, ret=" << rc << std::endl;
				SQLFreeHandle(SQL_HANDLE_STMT, hsmt);
				cleanBuffers(buffers);
				return DB_ERROR;
			}
			row_cof++;
			std::cout << "row_cof = " << row_cof << std::endl;
		}
	} while(SQL_SUCCEEDED(ret) && (rowCnt == FETCH_ROW_NUM)); 

	SQLFreeHandle(SQL_HANDLE_STMT, hsmt); 
	if((!SQL_SUCCEEDED(ret)) && (ret != SQL_NO_DATA))
	{        
		bool isReconnDB = false;
		ExtractError(requestID, SQL_HANDLE_DBC, m_hdbc, operName, isReconnDB);
		if(isReconnDB)
		{
			ReconnectDB(requestID, m_hdbc);
		}
		cleanBuffers(buffers);
		return DB_ERROR;
	}

	if(0 == row_cof) 
	{
		cleanBuffers(buffers); 
		return NAME_NOT_FOUND;
	}

	cleanBuffers(buffers);
	return DONE;
}

#endif
