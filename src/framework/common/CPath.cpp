#include "CPath.h"
#include <libgen.h>
#include <string.h>

CPath CPath::m_instance;

mp_int32 CPath::Init(mp_char* pFullFilePath)
{
	mp_char agentRoot[MAX_FULL_PATH + 1] = {0};
	
	char* pPath = dirname(pFullFilePath);
	if(NULL == pPath)
	{
		std::cout << "dirname failed, file path:" << pFullFilePath << std::endl;
		return COMMON_INERNAL_ERROR;
	}

	if(strlen(pPath) > MAX_FULL_PATH)
	{
		std::cout << "agent root path is too long, file path:" << pFullFilePath << std::endl;
		return COMMON_INERNAL_ERROR;
	}
	
	strncpy(agentRoot, pPath, strlen(pPath));

	mp_char *pTmp = strrchr(agentRoot, '/');
	if(NULL == pTmp)
	{
		std::cout << "strchr failed, file path:" << pFullFilePath << std::endl;
		return COMMON_INERNAL_ERROR;
	}

	mp_int32 index = pTmp - agentRoot;
	if(index < 0 && index > MAX_FULL_PATH)
	{
		agentRoot[index] = '\0';
	}

	m_agentRoot = agentRoot;

	return MP_SUCCESS;
}

