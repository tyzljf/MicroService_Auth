#include "CLoger.h"
#include <time.h>
#include <stdarg.h> 

CLoger CLoger::m_instance;

CLoger::CLoger()
{
	CMPThread::InitLock(&m_tLock);
	m_logLevel = DEFAULT_LOG_LEVEL;
	m_logCount = DEFAULT_LOG_COUNT;
	m_logMaxSize = DEFAULT_LOG_MAX_SIZE;
}

CLoger::~CLoger()
{
	CMPThread::DestoryLock(&m_tLock);
}

void CLoger::Init(const mp_string& strFileName, const mp_string& strFilePath, mp_int32 logLevel, mp_int32 logCount, mp_int32 logMaxSize)
{
	m_fileName = strFileName;
	m_filePath = strFilePath;
	setLogConf(logLevel, logCount, logMaxSize);
}

void CLoger::setLogConf(mp_int32 logLevel, mp_int32 logCount, mp_int32 logMaxSize)
{
	CAuthThreadLock tLock(m_tLock);
	setLogLevel(logLevel);
	setLogCount(logCount);
	setLogMaxSize(logMaxSize);
}

mp_int32 CLoger::setLogLevel(mp_int32 logLevel)
{
	if(LOG_TRACE > logLevel || LOG_CRIT < logLevel)
	{
		return MP_FAILED;
	}

	m_logLevel = logLevel;
	return MP_SUCCESS;
}

mp_int32 CLoger::setLogCount(mp_int32 logCount)
{
	if(0 > logCount)
	{
		return MP_FAILED;
	}

	m_logCount = logCount;
	return MP_SUCCESS;
}

mp_int32 CLoger::setLogMaxSize(mp_int32 logMaxSize)
{
	if(0 > logMaxSize)
	{
		return MP_FAILED;
	}

	m_logMaxSize = logMaxSize * 1024 * 1024;
	return MP_SUCCESS;
}

void CLoger::LogEx(const mp_string& module, mp_int32 level, std::size_t requestID, 
		const mp_string& functionName,	 mp_int32 fileLine,	const mp_char* format, ...)
{
	if(m_logLevel < level)
	{
		return;
	}

	//获取当前时间
	mp_char acMsgHead[10] = {0};
	mp_time now;
	mp_tm curTime;
	va_list vaArg;

	CMPTime::Time(&now);
	mp_int32 iRet = CMPTime::LocalTimeR(&now, &curTime);
	if(MP_FAILED == iRet)
	{
		return;
	}

	va_start(vaArg, format);
	CAuthThreadLock tLock(&m_tLock);

	//创建头部信息
	MakeHead(level, acMsgHead, sizeof(acMsgHead));

	//构造消息信息
	
	if(NULL == pFile)
	{
		pFile = openLogFile();
	}

	vfprintf(pFile, format, vaList);
	
	return;
}


mp_int32 CLoger::MakeHead(mp_int32 level, mp_char* acMsgHead, std::size_t acMsgLen)
{
	switch(level)
	{
		case LOG_TRACE:
			strncpy(acMsgHead, "[TRACE]", acMsgLen);
			break;
		case LOG_DEBUG:
			strncpy(acMsgHead, "[DEBUG]", acMsgLen);
			break;
		case LOG_INFO:
			strncpy(acMsgHead, "[INFO]", acMsgLen);
			break;
		case LOG_WARN:
			strncpy(acMsgHead, "[WARN]", acMsgLen);
			break;
		case LOG_CRIT:
			strncpy(acMsgHead, "[CRIT]", acMsgLen);
			break;
		default:
			strncpy(acMsgHead, "", acMsgLen);
			break;
	}

	return MP_SUCCESS;
}
