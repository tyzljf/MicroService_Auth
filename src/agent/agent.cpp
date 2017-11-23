#include <iostream>
#include <unistd.h>
#include "StackTracer.h"

std::string AgentLogName = "agent.log";

/****************************************************************************
函数名称:ParseLogName
函数功能:解析日志文件名称
输入:		
	path:可执行文件所在路径
输出:无
返回值:	无
特殊说明:程序名称作为日志文件名称
****************************************************************************/
void ParseLogName(const std::string& path)
{
	size_type pos = path.rfind("/");
	AgentLogName = path.substr(pos);
}

void DaemonInit()
{
    

}


int main(int argc, char *argv[])
{
	ParseLogName(argv[0]);
	//捕获程序异常退出时的堆栈信息
	StackTracer stackTracer; 

	DaemonInit();
	

	for(;;)
	{
		sleep(1);
	}

	return 0;
}

