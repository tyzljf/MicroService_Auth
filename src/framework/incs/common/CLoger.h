#ifndef __CLOGER_H
#define __CLOGER_H

#include "Types.h"

#define LOG_TRACE 	1
#define LOG_DEBUG 	2
#define LOG_INFO 	3
#define LOG_WARN	4
#define LOG_CRIT	5

#define DEFAULT_LOG_LEVEL 3
#define DEFAULT_LOG_COUNT 5
#define DEFAULT_LOG_MAX_SIZE (30 * 1024 * 1024) //30MB

class CLoger
{
public:
	CLoger& GetInstance()
	{
		return m_instance;
	}

	~CLoger();

	void Init(const mp_string& strFileName, const mp_string& strFilePath, mp_int32 logLevel, mp_int32 logCount, mp_int32 logMaxSize);

	void LogEx(const mp_string& module, mp_int32 level, std::size_t requestID, 
		const mp_string& functionName,	 mp_int32 fileLine,	const mp_char* format, ...);
private:
	CLoger();
	void setLogConf(mp_int32 logLevel, mp_int32 logCount, mp_int32 logMaxSize);
	mp_int32 setLogLevel(mp_int32 logLevel);
	mp_int32 setLogCount(mp_int32 logCount);
	mp_int32 setLogMaxSize(mp_int32 logMaxSize);
	
	static CLoger m_instance;
	thread_mutex_t m_tLock;
	
	mp_string m_fileName;	//日志文件名
	mp_string m_filePath;	//日志文件路径
	mp_int32  m_logLevel;	//日志级别
	mp_int32  m_logCount;	//保留日志个数 
	mp_int32  m_logMaxSize;	//日志大小
};


class LogProxy
{
public:
	LogProxy(const mp_string& module, mp_int32 level, std::size_t requestID, const mp_string& funcName,
		mp_int32 fileLine, const mp_char* format, ...):
		m_module(module),
		m_level(level),
		m_requestID(requestID),
		m_funcName(funcName)
		{
		}
		
	~LogProxy()
	{
		CLoger::GetInstance().LogEx(module,mp_int32 level, size_t requestID,const mp_string & functionName,mp_int32 fileLine,const mp_char * format,...)
	}

private:
	mp_string m_module;
	mp_int32  m_level;
	std::size_t m_requestID;
	mp_string m_funcName;
	mp_int32  m_fileLine;
	
};

//#define Loger_noid() 

#define