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
	//1. ��ʼ�����ݿ�ӿ�
	m_IamDBInterface = GetIamDBObj();
	if(NULL == m_IamDBInterface)
	{
		std::cout << "init iam db interface failed." << std::endl;
		return 1;
	}

	//2. ��ʼ���м��
	m_IamUIInterface = GetIamUiObj(m_IamDBInterface);
	if(NULL == m_IamDBInterface)
	{
		std::cout << "init iam db interface failed." << std::endl;
		return 1;
	}

	//3. ע��URL
	
	
}
