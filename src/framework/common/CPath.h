#ifndef __CPATH_H
#define __CPATH_H

#include "Types.h"
#include "CommonConst.h"

#define MAX_FULL_PATH  1024

class CPath
{
public:
	static CPath& GetInstance()
	{
		return m_instance;	
	}

	~CPath()
	{
	}

	mp_string GetAgentRoot()
	{
		return m_agentRoot;
	}

	mp_int32 Init(mp_char* pFullFilePath);

	mp_string GetLogPath()
	{
		return GetAgentRoot() + PATH_SEPARATOR + "logs";
	}
	

private:
	CPath()
	{
	}
	CPath(const CPath& rhs);
	CPath& operator=(const CPath& rhs);
	
	static CPath m_instance;
	mp_string m_agentRoot;
};


#endif