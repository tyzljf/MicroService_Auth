#define __CMP_THREAD_H
#define __CMP_THREAD_H

#include "Types.h"

class CMPThread
{
public:
	static mp_int32 InitLock(thread_mutex_t* thread);
	static mp_int32 DestoryLock(thread_mutex_t* thread);
	static mp_int32 CreateThread(thread_t* threadId, void*(*routine)(void *), void* arg);
};

class CAuthThreadLock
{
public:
	CAuthThreadLock(thread_mutex_t* pMutex);
	~CAuthThreadLock();

private:
	thread_mutex_t* m_mutex;
};


#endif