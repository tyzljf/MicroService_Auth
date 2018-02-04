#include "Communication.h"
#include <stdio.h>

#define DEFAULT_REQUEST_NUM 100

Communication Communication::m_instance;

Communication& Communication::GetInstance()
{
	return m_instance;
}

Communication::Communication()
{
	m_NeedExit = false;
	
	CMPThread::InitLock(&m_ReqHandlerMutex);
	CMPThread::InitLock(&m_ReqMsgMutex);
	CMPThread::InitLock(&m_RspMsgMutex);
}

Communication::~Communication()
{
	DeleteRequest(m_ReqHandlers);
	
	CMPThread::DestoryLock(&m_ReqHandlerMutex);
	CMPThread::DestoryLock(&m_ReqMsgMutex);
	CMPThread::DestoryLock(&m_RspMsgMutex);
}

void Communication::SetReciveThreadStatus(ThreadStatus threadStatus)
{
	m_RecvThreadStatus = threadStatus;
}

void Communication::SetSendThreadStatus(ThreadStatus threadStatus)
{
	m_SendThreadStatus = threadStatus;
}

bool Communication::NeedExit()
{
	return m_NeedExit;
}

mp_uint32 Communication::Init()
{
	//��ʼ��FCGX������Ϣ
	mp_int ret = FCGX_Init();
	if(MP_SUCCESS != ret)
	{
		std::cout << "Call FCGX_Init failed, ret=" << ret << std::endl;
		ret = COMMON_INERNAL_ERROR;
		return ret;
	}

	//����FCGX�����˿�
	//TODO: �������ж�ȡ�˿���Ϣ
	mp_int port = 8088;
	char buffer[20] = {0};
	mp_string strAddr;
	
	ret = snprintf(buffer, sizeof(buffer), "127.0.0.1:%d", port);
	if(ret < 0)
	{
		std::cout << "snprintf failed, ret=" << ret << std::endl;
		ret = COMMON_INERNAL_ERROR;
		return ret;
	}

	strAddr(buffer);
	mp_int handler = FCGX_OpenSocket(strAddr.c_str(), 0);
	if(handler < 0)
	{
		std::cout << "FCGX_OpenSocket failed, ret=" << ret << std::endl;
		ret = COMMON_INERNAL_ERROR;
		return ret;
	}

	ret = InitRequest(handler);
	if(MP_SUCCESS != ret)
	{
		std::cout << "InitRequest failed, ret=" << ret << std::endl;
		OS_Close(handler, MP_TRUE);
		return COMMON_INERNAL_ERROR;
	}
	
	ret = CMPThread::CreateThread(&m_ReqThread, RecvThreadFunc, this);
	if(MP_SUCCESS != ret)
	{
		//��ӡ��־��Ϣ
		OS_Close(handler, MP_TRUE);
		ret = COMMON_INERNAL_ERROR;
		return ret;
	}
	
	ret = CMPThread::CreateThread(&m_ReqThread, SendThreadFunc, this);
	if(MP_SUCCESS != ret)
	{
		//��ӡ��־��Ϣ
		OS_Close(handler, MP_TRUE);
		ret = COMMON_INERNAL_ERROR;
		return ret;
	}

	return MP_SUCCESS;
}

mp_int Communication::InitRequest(mp_int handler)
{
	mp_int ret;
	
	//����Ԥ����100��FCGX_Request����
	//TODO: DEFAULT_REQUEST_NUM ��������ļ��ж�ȡ
	for(int i = 0; i < DEFAULT_REQUEST_NUM; i++)
	{
		req_handler_info_t reqHandler;
		reqHandler.req = NULL;
		reqHandler.used = false;
		m_ReqHandlers.push_back(reqHandler);
	}

	//��ӡFCGX_Request�������

	for(vector<req_handler_info_t>::iterator it = m_ReqHandlers.begin(); it != m_ReqHandlers.end(); it++)
	{
		FCGX_Request* fcgxRequest = NULL;
		NEW_PATCH(fcgxRequest, FCGX_Request);
		if(NULL == fcgxRequest)
		{	
			//��ӡ��־��Ϣ
			ret = COMMON_INERNAL_ERROR;
			break;
		}
		FCGX_InitRequest(m_ReqHandlers[i].req, handler, 0);
		m_ReqHandlers[i].req = fcgxRequest;
	}

	if(MP_SUCCESS != ret)
	{
		//��ӡ��־
		DeleteRequest(m_ReqHandlers);
	}

	return MP_SUCCESS;
}

void Communication::DeleteRequest(vector<req_handler_info_t>& reqHandlers)
{
	CAuthThreadLock tLock(&m_ReqHandlerMutex);
	for(vector<req_handler_info_t>::iterator it = reqHandlers.begin(); it != reqHandlers.end(); it++)
	{
		if(reqHandlers[i].req)
		{
			delete reqHandlers[i].req;
			reqHandlers.req = NULL;
		}
	}
}

void Communication::RecvThreadFunc(void *arg)
{
	Communication* pInstance = (Communication*)arg;

	pInstance->SetReciveThreadStatus(THREAD_STATUS_RUNNING);
	while(pInstance->NeedExit())
	{
		FCGX_Request* pReq = pInstance->GetFCGXReq();
		if(NULL == pReq)
		{
			//��ӡ��־����˯��2����
			CMPTime::DoSleep(2);
			continue;
		}
	
		mp_int ret = FCGX_Accept_r(pReq);
		if(ret < 0)
		{
			//��ӡ��־����˯��2����
			pInstance->ReleaseRequest(pReq);
			continue;
		}

		//��ӡHTTPͷ����Ϣ
		CRequestMsg cRequestMsg(pReq);

		ReciveHandlerMsg(pInstance, pReq);
	}

	pInstance->SetReciveThreadStatus(THREAD_STATUS_EXIT);
}

void Communication::SendThreadFunc(void *arg)
{
	Communication* pInstance = (Communication*)arg;

	while(pInstance->NeedExit())
	{
		
	}

}

FCGX_Request* Communication::GetFCGXReq()
{
	CAuthThreadLock tLock(&m_ReqHandlerMutex);
	for(vector<req_handler_info_t>::iterator it = m_ReqHandlers.begin(); it != m_ReqHandlers.end(); it++)
	{
		if(!it->used)
		{
			it->used = MP_TRUE;
			return it->req;
		}
	}

	//��ӡ��־��Ϣ
	return NULL;
}

void Communication::ReleaseRequest(FCGX_Request* pReq)
{
	CAuthThreadLock tLock(&m_ReqHandlerMutex);
	for(vector<req_handler_info_t>::iterator it = m_ReqHandlers.begin(); it != m_ReqHandlers.end(); it++)
	{
		if(*it == pReq)
		{
			it->used = MP_FALSE;
			break;
		}
	}
}

void Communication::ReciveHandlerMsg(Communication* pInstance, FCGX_Request* pFcgxReq)
{
	if(NULL == pInstance || NULL == pFcgxReq)
	{
		return;
	}

	CRequestMsg* requestMsg = new (std::nothrow) CRequestMsg(pFcgxReq);
	if(NULL == requestMsg)
	{
		//��ӡ��־������HTTP��Ӧ��Ϣ,�ͷ�����
		SendFailedMsg(pInstance, pFcgxReq, SC_NO_ACCEPTABLE, MP_FALSE);
		return;
	}

	CRequestMsg* responseMsg = new (std::nothrow) CRequestMsg(pFcgxReq);
	if(NULL == responseMsg)
	{
		//��ӡ��־������HTTP��Ӧ��Ϣ���ͷ�FCGX_Request����
		SendFailedMsg(pInstance, pFcgxReq, SC_NO_ACCEPTABLE, MP_FALSE);
		delete requestMsg;
		requestMsg = NULL;
		return;
	}


	mp_int ret = responseMsg->Parse();
	if(MP_SUCCESS != ret)
	{
		//��ӡ��־������HTTP��Ӧ��Ϣ���ͷ�FCGX_Request����
		SendFailedMsg(pInstance, pFcgxReq, SC_NO_ACCEPTABLE, MP_FALSE);
		delete requestMsg;
		requestMsg = NULL;
		delete responseMsg;
		responseMsg = NULL;
		return;
	}

	msg_pair_t msg_pair(requestMsg, responseMsg);
	pInstance->PushReqQueueMsg(msg_pair);
}

void Communication::PushReqQueueMsg(msg_pair_t& msg_pair)
{
	if(m_CurReq.load() >= /*�������ļ��ж�ȡ*/)
	{
		//�ͷ���Դ������HTTP��Ӧ��Ϣ
		msg_pair.rsp->SetHttpStatus(SC_NO_ACCEPTABLE);
		msg_pair.rsp->SetRetCode(MP_FALSE);
		msg_pair.rsp->Send();
		ReleaseRequest(msg_pair.rsp->pFcgxReq);
		delete msg_pair.req;
		msg_pair.req = NULL;
		delete msg_pair.rsp;
		msg_pair.rsp = NULL;
		return;
	}

	{
		CAuthThreadLock theadLock(m_ReqMsgMutex);
		m_ReqMsgQueue.push_back(msg_pair);
		m_CurReq++;
		//��ӡ��־
	}
}

msg_pair_t Communication::PopReqMsgQueue()
{
	CAuthThreadLock tLock(&m_ReqHandlerMutex);
	for(vector<msg_pair_t>::iterator it = m_ReqMsgQueue.begin(); it != m_ReqMsgQueue.end(); it++)
	{
		msg_pair_t msg_pair = *it;
		return msg_pair;
	}
}

void Communication::PushRspMsgQueue(msg_pair_t& msg_pair)
{
	CAuthThreadLock tLock(&m_RspMsgMutex);
	m_RspMsgQueue.push_back(msg_pair);
}

msg_pair_t& Communication::PopRspMsgQueue()
{
	CAuthThreadLock tLock(&m_RspMsgMutex);
	for(vector<msg_pair_t>::iterator it = m_RspMsgQueue.begin(); it != m_RspMsgQueue.end(); it++)
	{
		if(it->rsp.is)
	}
}


void Communication::SendFailedMsg(Communication* pInstance, FCGX_Request* pFcgxReq, mp_int32 httpStatus, mp_int32 retCode)
{
	CResponseMsg rsp(pFcgxReq);
	rsp.SetRetCode(retCode);
	rsp.SetHttpStatus(httpStatus);
	rsp.Send();
	pInstance->ReleaseRequest(pFcgxReq);
}
