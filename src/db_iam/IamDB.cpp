#include "IamDB.h"

IamDBInterface* CreateDBInterface()
{
	return new(std::norow)IamDB();
}

bool IamDB::Init(std::size_t size, const std::string& dataSource)
{
	if(!m_connMgr.Init(size, dataSource))
	{
		std::cout << "Init db connection manager failed." << std::endl;
		return false;
	}

	if(!m_jsonConverter.Init(&m_adapterOpt))
	{
		std::cout << "Init json converter failed." << std::endl;
		return false;
	}

	return true;
}


ResponseAddEvent IamDB::AddEvent(const RequestAddEvent& reqAddEvent)
{
	ResponseAddEvent response;
	response.rc = ADAP_OK;


	return response;
}

