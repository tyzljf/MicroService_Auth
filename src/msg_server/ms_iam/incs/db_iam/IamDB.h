#ifndef __IAMDB_H
#define __IAMDB_H

#include "DBConnManager.h"
#include "IamDBInterface.h"
#include "JsonQueryConverter.h"

#define FUNCTION_RETURN(ret)	\
	{							\
		response.rc = ret;		\
		return response;		\
	}

class IamDB : public IamDBInterface
{
public:
	IamDB();
	virtual ~IamDB();
	
	virtual ResponseAddUser  		AddUser(const RequestAddUser& reqAddUser);
	virtual ResponseQueryUser 		QuerySingleUser(const RequestQueryUser& reqUser);
	virtual bool Init(std::size_t size, std::string& dataSource);

private:
	DBConnManager m_connMgr;
	DBAdapterOpt  m_adapterOpt;
	JsonQueryConverter m_jsonConverter;
};



#endif