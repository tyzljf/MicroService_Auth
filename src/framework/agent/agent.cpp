#include <iostream>
#include <unistd.h>
#include "StackTracer.h"
#include "TaskPool.h"

std::string AgentLogName = "agent.log";
static bool gExitFlag = false;

/****************************************************************************
函数名称:ParseLogName
函数功能:解析日志文件名称
输入:		
	path:可执行文件所在路径
输出:无
返回值:	无
特殊说明:程序名称作为日志文件名称
****************************************************************************/
void ParseLogName(const char* path)
{
	std::string filePath = path;
	std::string fileName = filePath.substr(filePath.rfind("/")+1);
	AgentLogName = fileName + ".log";
}

/****************************************************************************
函数名称:DaemonInit
函数功能:创建守护进程
输入: 无
输出:无
返回值:	无
特殊说明:
****************************************************************************/

void DaemonInit()
{
    pid_t pid;

	if(0 == (pid = fork()))
	{
		std::cout << "create a child process sussess." << std::endl;
		return;
	}
	else if(pid > 0)
	{
		std::cout << "parent process exit." << std::endl;
		exit(0);
	}
	else
	{
		std::cout << "fork child process failed." << std::endl;
		exit(1);
	}
}

mp_int32 AgentInit(const mp_char* pFullFilePath, CTaskPool& taskPool)
{
	//初始化程序路径
	mp_int32 ret = CPath::GetInstance().Init(pFullFilePath);
	if(MP_SUCCESS != ret)
	{
		std::cout << "CPath init failed, ret=" << ret << std::endl;
		return COMMON_INERNAL_ERROR;
	}

	//从配置文件中获取日志级别、日志数量、日志大小
	CLoger::GetInstance().Init(strFileName, strFilePath, logLevel, logCount,logMaxSize);

	//初始化通信模块
	mp_uint32 ret = Communication::GetInstance().Init();
	if(MP_SUCCESS != ret)
	{
		return MP_FAILED;
	}

	ret = taskPool.Init();
	if(MP_SUCCESS != ret)
	{

		return MP_FAILED;
	}
	
	return MP_SUCCESS;
}


int main(int argc, char *argv[])
{
	CTaskPool	taskPool;
	ParseLogName(argv[0]);
	//捕获程序异常退出时的堆栈信息
	StackTracer stackTracer; 

	DaemonInit();
	
	mp_int ret = AgentInit(argv[0], taskPool);
	if(MP_SUCCESS != ret)
	{
		std::cout << "Agent init failed, ret:" << ret << std::endl;
		return;
	}

	for(;;)
	{
		if(gExitFlag)
		{
			exit(0);
		}
		CMPTime::DoSleep(2);
	}

	return MP_SUCCESS;
}

