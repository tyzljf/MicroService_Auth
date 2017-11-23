#include <iostream>
#include <unistd.h>
#include "StackTracer.h"

std::string AgentLogName = "agent.log";

/****************************************************************************
��������:ParseLogName
��������:������־�ļ�����
����:		
	path:��ִ���ļ�����·��
���:��
����ֵ:	��
����˵��:����������Ϊ��־�ļ�����
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
	//��������쳣�˳�ʱ�Ķ�ջ��Ϣ
	StackTracer stackTracer; 

	DaemonInit();
	

	for(;;)
	{
		sleep(1);
	}

	return 0;
}

