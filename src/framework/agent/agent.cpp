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

int main(int argc, char *argv[])
{
	TaskPool taskPool;
	ParseLogName(argv[0]);
	//捕获程序异常退出时的堆栈信息
	StackTracer stackTracer; 

	DaemonInit();
	
	int ret = AgentInit(argv[0], taskPool);
	if(ret < 0)
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
		sleep(1);
	}

	return 0;
}

