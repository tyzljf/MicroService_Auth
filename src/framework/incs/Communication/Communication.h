#include <vector>
#include <queue>
#include "Fcgiapp.h"
#include "CommunicationMsg.h"

typedef struct tag_handler_info_t
{
	FCGX_Request* req;
	bool used;
}req_handler_info_t;

typedef struct tag_msg_pair_t
{
	CRequestMsg* req;
	CResponseMsg* rsp;
	tag_msg_pair_t()
	{
		req = NULL;
		rsp = NULL;
	}
	tag_msg_pair_t(CRequestMsg* pReq, CResponseMsg* pRsp)
	{
		req = pReq;
		rsp = pRsp;
	}
}msg_pair_t;

enum ThreadStatus
{
	THREAD_STATUS_IDLE,
	THREAD_STATUS_RUNNING,
	THREAD_STATUS_EXIT
};

class Communication
{
public:
	static Communication& GetInstance();
	~Communication();

	bool Init();

private:
	static Communication m_instance;
	Communication();
	Communication(const Communication& rhs);
	Communication& operator=(const Communication& rhs);

	bool InitRequest(mp_int handler);
	void DeleteRequest(vector<req_handler_info_t>& reqHandlers);

	void SetReciveThreadStatus(ThreadStatus threadStatus);

	vector<req_handler_info_t> m_ReqHandlers;
	thread_mutex_t m_ReqHandlerMutex;
	thread_t	m_ReqThread;
	
	vector<msg_pair_t> m_ReqMsgQueue;
	thread_mutex_t     m_ReqMsgMutex;
	vector<msg_pair_t> m_RspMsgQueue;
	thread_mutex_t	   m_RspMsgMutex;

	volatile mp_int m_RecvThreadStatus;
	volatile mp_int m_SendThreadStatus;
	
	bool m_NeedExit;
	automic<mp_int> m_CurReq;
};
