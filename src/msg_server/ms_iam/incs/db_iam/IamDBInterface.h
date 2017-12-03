#ifndef __IAMDB_INTERFACE_H
#define __IAMDB_INTERFACE_H

#include <iostream>
#include <vector>
#include "Types.h"

enum ReturnCode
{
	ADAP_OK,
	ADAP_DB_ERROR,
	ADAP_USERS_MAX_NUM,
	ADAP_USER_EXIST,
	APAP_USER_NOT_EXIST,
	ADAP_LAST_RC
};

struct UserInfo_t
{
public:
	std::string userName;
	std::string password;
};

class RequestAddUser
{
public:
	const UserInfo_t& UserInfo() const {return userInfo;}
	void UserInfo(const UserInfo_t& info) {userInfo = info;}
	
	std::size_t requestID;
	UserInfo_t  userInfo;
};

class ResponseAddUser
{
public:
	ReturnCode rc;
	uint64_t userid;
};

class RequestQueryUser
{
public:
	const std::string& Name() const {return name;}
	void Name(const std::string& mName) {name = mName;}
	
	std::size_t requestID;
	std::string name;
};

class ResponseQueryUser
{
public:
	ReturnCode rc;
	std::vector<UserInfo_t> users;
};

class IamDBInterface
{
public:
	//封装业务相关的数据库操作接口，如:
	virtual ResponseAddUser 	 	AddUser(const RequestAddUser& reqAddUser) = 0;
	virtual ResponseQueryUser 		QuerySingleUser(const RequestQueryUser& reqUser) = 0;

public:
	virtual bool Init(std::size_t size, std::string& dataSource) = 0;
};

extern "C" IamDBInterface* CreateDBInterface();

#endif
