#include <iostream>
#include <unistd.h>
#include "StackTracer.h"
#include "TaskPool.h"

std::string AgentLogName = "agent.log";
static bool gExitFlag = false;

/****************************************************************************
��������:ParseLogName
��������:������־�ļ�����
����:		
	path:��ִ���ļ�����·��
���:��
����ֵ:	��
����˵��:����������Ϊ��־�ļ�����
****************************************************************************/
void ParseLogName(const char* path)
{
	std::string filePath = path;
	std::string fileName = filePath.substr(filePath.rfind("/")+1);
	AgentLogName = fileName + ".log";
}

/****************************************************************************
��������:DaemonInit
��������:�����ػ�����
����: ��
���:��
����ֵ:	��
����˵��:
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
	//��������쳣�˳�ʱ�Ķ�ջ��Ϣ
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

