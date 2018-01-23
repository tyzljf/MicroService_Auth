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

mp_int32 AgentInit(const mp_char* pFullFilePath, CTaskPool& taskPool)
{
	//��ʼ������·��
	mp_int32 ret = CPath::GetInstance().Init(pFullFilePath);
	if(MP_SUCCESS != ret)
	{
		std::cout << "CPath init failed, ret=" << ret << std::endl;
		return COMMON_INERNAL_ERROR;
	}

	//�������ļ��л�ȡ��־������־��������־��С
	CLoger::GetInstance().Init(strFileName, strFilePath, logLevel, logCount,logMaxSize);

	//��ʼ��ͨ��ģ��
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
	//��������쳣�˳�ʱ�Ķ�ջ��Ϣ
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

