#ifndef __DBCONN_H
#define __DBCONN_H

#include <sqlext.h>
#include <sql.h>
#include "DBAdapterOpt.h"
#include <set>

using namespace std;

enum CHECK_RC_E
{
	DONE,
	SYSTEM_OUT_MEM,
	NAME_NOT_FOUND,
	DB_ERROR	
};

class DBConn
{
public:
	DBConn(SQLHENV &henv, std::string& dataSource);
	virtual ~DBConn();
	
	bool Release();
	bool Init();

	CHECK_RC_E dbCommand(std::size_t requestID, std::string& command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect = NULL);
	CHECK_RC_E dbCommand(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect = NULL);
	
	CHECK_RC_E IntQuery(std::size_t requestID, std::string& command, std::string param, uint64_t* intPtr);
	CHECK_RC_E IntQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::string& param, uint64_t* intPtr);
	CHECK_RC_E IntQuery(std::size_t requestID, std::string& command, std::vector<ParamToBind_t>& params, uint64_t* intPtr);
	CHECK_RC_E IntQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint64_t* intPtr);

	CHECK_RC_E StringQuery(std::size_t requestID, std::string& command, std::vector<ParamToBind_t>& params, std::string& strRef);
	CHECK_RC_E StringQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::string& strRef);

	CHECK_RC_E dbQuery(std::size_t requestID, std::string& command, std::vector<ParamToBind_t>& params, std::vector<QueryResult_t>& result);
	CHECK_RC_E dbQuery(std::size_t requestID, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::vector<QueryResult_t>& result);

	CHECK_RC_E StartTranscation(std::size_t requestID);
	CHECK_RC_E EndTranscation(std::size_t requestID);
	CHECK_RC_E RollbackRranscation(std::size_t requestID);
private:
	CHECK_RC_E generalDbCommand(std::size_t requestID,  SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint_t* numRowAffect);
	void ExtractError(std::size_t requestID, SQLSMALLINT type, SQLHDBC& hdbc, const char* operName, bool& isReconnDB);
	bool ReconnectDB(std::size_t requestID, SQLHDBC& hdbc);
	CHECK_RC_E bindParams(std::size_t requestID, SQLHDBC& hdbc, HSTMT& hsmt, std::vector<ParamToBind_t>& params, const char* operName);
	CHECK_RC_E checkReturnRCResponseKill(std::size_t requestID, SQLHDBC& hdbc, SQLHDBC& hsmt, SQLRETURN ret, const char* operName);
	void printSQLGetDiagRec(std::size_t requestID, SQLSMALLINT type, SQLHANDLE& handle, const char* operName);

	CHECK_RC_E generalIntQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, uint64_t* intPtr);
	CHECK_RC_E generalStringQuery(std::size_t requestID, SQLHDBC& hdbc, SQLCHAR* command, std::vector<ParamToBind_t>& params, std::string& strRef);

	CHECK_RC_E bindColums(std::size_t requestID, SQLHDBC& hdbc, HSTMT& hsmt, std::vector<QueryResult_t>& result, std::vector<char*>& cString, SQLLEN* rLenAttr);
	void cleanTmpBuffer(SQLLEN* rLenAttr, std::vector<QueryResult_t>&result, std::vector<char*>& cString);
	int processNullData(std::size_t requestID, std::vector<QueryResult_t>& result, SQLLEN* rLenAttr);
	void processStringData(std::size_t requestID, std::vector<QueryResult_t>&result, std::vector<char*>& cString, SQLLEN* rLenAttr);

	static std::set<std::string> initRestartSignals();
	
private:
	static std::set<std::string> m_restartSignals;
	SQLHENV m_henv; //定义环境句柄
	SQLHDBC m_hdbc;   //定义数据连接句柄
	std::string m_dataSource;
};

#endif
