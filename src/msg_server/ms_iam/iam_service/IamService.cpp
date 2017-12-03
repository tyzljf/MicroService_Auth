#include "IamService.h"

IamService::IamService()
{

}

IamService::~ IamService()
{

}

IamDBInterface* IamService::GetIamDBObj()
{
	m_IamDBInterface = CreateDBInterface();

}
int IamService::InitMicroService()
{
	//1. 初始化数据库接口
	m_IamDBInterface = GetIamDBObj();
	if(NULL == m_IamDBInterface)
	{
		std::cout << "init iam db interface failed." << std::endl;
		return 1;
	}

	//2. 初始化中间层
	m_IamUIInterface = GetIamUiObj(m_IamDBInterface);
	if(NULL == m_IamDBInterface)
	{
		std::cout << "init iam db interface failed." << std::endl;
		return 1;
	}

	//3. 注册URL
	
	
}
