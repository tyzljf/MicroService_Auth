#ifndef __IAM_SERVICE_H
#define __IAM_SERVICE_H

class IamService
{
public:
	IamService();
	virtual ~IamService();


public:
	int InitMicroService();

private:
	IamDBInterface* GetIamDBObj();
	


private:
	IamDBInterface *m_IamDBInterface;

};

#endif
