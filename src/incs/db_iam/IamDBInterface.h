#ifndef __IAMDB_INTERFACE_H
#define __IAMDB_INTERFACE_H

enum CHECK_RC_E
{
	

};

class RequestAddEvent
{

};

class ResponseAddEvent
{
public:
	CHECK_RC_E rc;
};


class IamDBInterface
{
public:
	//封装业务相关的数据库操作接口，如:
	virtual ResponseAddEvent AddEvent(const RequestAddEvent& reqAddEvent) = 0;

public:
	bool Init(std::size_t size, const std::string& dataSource) = 0;
};

extern "C" IamDBInterface* CreateDBInterface();

#endif