#include "IamDB.h"

IamDBInterface* CreateDBInterface()
{
	return new(std::nothrow) IamDB();
}

IamDB::IamDB()
{

}

IamDB::~IamDB()
{

}

bool IamDB::Init(std::size_t size, std::string& dataSource)
{
	if(!m_connMgr.Init(size, dataSource))
	{
		std::cout << "Init db connection manager failed." << std::endl;
		return false;
	}

	if(JSON_PARSE_OK != m_jsonConverter.Init(&m_adapterOpt))
	{
		std::cout << "Init json converter failed." << std::endl;
		return false;
	}

	return true;
}


ResponseAddUser IamDB::AddUser(const RequestAddUser& reqAddUser)
{
	ResponseAddUser response;
	response.rc = ADAP_OK;
	std::size_t requestID = reqAddUser.requestID;
	UserInfo_t userInfo = reqAddUser.UserInfo();

	//1. 获取连接对象	
	CHECK_RC_E checkRC;
	std::shared_ptr<DBConn> conn = m_connMgr.GetConnection();

	uint64_t userCount;
	std::string	command = "select count(*) from users;";
	checkRC = conn->IntQuery(requestID, command, std::string(""), &userCount);
	if(DONE != checkRC)
	{
		std::cout << "get user count failed, checkRC:" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR)
	}
	std::cout << "the number of current user is " << userCount << std::endl;

	uint64_t maxNumUsers = 200; //get the value of the max user number from config file.
	if(userCount > maxNumUsers)
	{
		std::cout << "The number of users reached the maximum." << std::endl;
		FUNCTION_RETURN(ADAP_USERS_MAX_NUM)
	}

	command = "select password from users where name = ?";
	std::string password;
	std::vector<ParamToBind_t> params;	
	m_adapterOpt.AddStringParam(params, &userInfo.userName);
	checkRC = conn->StringQuery(requestID, command, params, password);
	if(DONE == checkRC)
	{
		std::cout << "The user is exist."<< std::endl;
		FUNCTION_RETURN(ADAP_USER_EXIST)
	}
	if(NAME_NOT_FOUND != checkRC)
	{
		std::cout << "Get the user failed, checkRC=" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR)
	}
	
	//begin transcation
	checkRC = conn->StartTranscation(requestID);
	if(DONE != checkRC)
	{
		std::cout << "Start transcation failed, checkRC=" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR) 
	}
	
	command = "insert into users (name, password) values(?,?);";
	params.clear();
	m_adapterOpt.AddStringParam(params, &(userInfo.userName));
	m_adapterOpt.AddStringParam(params, &(userInfo.password));

	checkRC = conn->dbCommand(requestID, command, params);
	if(DONE != checkRC)
	{
		std::cout << "Add user failed, checkRC=" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR)
	}

	uint64_t userID;
	command = "select id from users where name = ?";
	checkRC = conn->IntQuery(requestID, command, userInfo.userName, &userID);
	if(DONE != checkRC)
	{
		std::cout << "Get the user id failed, checkRC:" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR)
	}
	std::cout << "The id of current user is " << userID << std::endl;

	checkRC = conn->EndTranscation(requestID);
	if(DONE != checkRC)
	{
		std::cout << "End transcation failed, checkRC=" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR) 
	}

	return response;
}

ResponseQueryUser IamDB::QuerySingleUser(const RequestQueryUser& reqUser)
{
	ResponseQueryUser response;
	response.rc = ADAP_OK;
	std::size_t requestID = reqUser.requestID;
	std::string userName = reqUser.Name();
	
	//1. 获取连接对象	
	CHECK_RC_E checkRC;
	std::shared_ptr<DBConn> conn = m_connMgr.GetConnection();

	uint64_t userCount;
	std::string	command = "select name, password from users where name = ?;";

	std::vector<ParamToBind_t> params;
	m_adapterOpt.AddStringParam(params, &userName);

	std::vector<QueryResult_t> result;
	std::string name, password;
	m_adapterOpt.AddDefStringParam(result, &name);
	m_adapterOpt.AddDefStringParam(result, &password);
	
	checkRC = conn->dbQuery(requestID, command, params, result);
	if(NAME_NOT_FOUND == checkRC)
	{
		std::cout << "The user is not exist, username:" << userName << std::endl;
		FUNCTION_RETURN(APAP_USER_NOT_EXIST)
	}
	if(DONE != checkRC)
	{
		std::cout << "Get the user information failed, checkRC=" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR)
	}

	std::cout << "name:" << name << std::endl;
	std::cout << "password:" << password << std::endl;

	response.users.push_back(UserInfo_t());
	response.users[0].userName = name;
	response.users[0].password = password;

	return response;
}

ResponseQueryAllUsers IamDB::QueryAllUsers(const RequestQueryUser& reqUser)
{
	ResponseQueryAllUsers response;
	response.rc = ADAP_OK;
	std::size_t requestID = reqUser.requestID;
	
	//1. 获取连接对象	
	CHECK_RC_E checkRC;
	std::shared_ptr<DBConn> conn = m_connMgr.GetConnection();
	std::string	command = "select id, name, password from users;";

	std::vector<ParamToBind_t> params;
	checkRC = conn->flexQuery(requestID, command, params, response.users);
	if(NAME_NOT_FOUND == checkRC)
	{
		std::cout << "The user is empty."<< std::endl;
		FUNCTION_RETURN(APAP_USER_NOT_EXIST)
	}
	if(DONE != checkRC)
	{
		std::cout << "Get the user information failed, checkRC=" << checkRC << std::endl;
		FUNCTION_RETURN(ADAP_DB_ERROR)
	}

	return response;
}

