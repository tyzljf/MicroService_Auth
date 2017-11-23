#ifndef __IAMDB_H
#define __IAMDB_H

#include "DBConnManager.h"
#include "DBAdapterOpt.h"


enum
{
	ADAP_OK,
};

class IamDB : public IamDBInterface
{
public:
	virtual ResponseAddEvent AddEvent(const RequestAddEvent& reqAddEvent);

	virtual bool Init(std::size_t size, const std::string& dataSource);

private:
	DBConnManager m_connMgr;
	DBAdapterOpt  m_adapterOpt;
	JsonQueryConverter m_jsonConverter;
};



#endif