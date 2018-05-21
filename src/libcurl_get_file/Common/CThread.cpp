#include "Types.h"

#include "CThread.h"

int32_t CThread::InitLock(thread_mutex_t* thread)
{
    int32_t ret = pthread_mutex_init(thread, NULL);
    if(ret != 0)
    {
        return DP_FAILED;
    }

    return DP_SUCCESS;
}

int32_t CThread::DestoryLock(thread_mutex_t* thread)
{
    int32_t ret = pthread_mutex_destroy(thread);
    if(ret != 0)
    {
        return DP_FAILED;
    }

    return DP_SUCCESS;
}

int32_t CThread::CreateThread(thread_t* threadId, void*(*routine)(void *), void* arg)
{
    int32_t ret = pthread_create(threadId, NULL, routine, arg);
    if(ret != 0)
    {
        return DP_FAILED;
    }

    return DP_SUCCESS;
}

CAuthThreadLock::CAuthThreadLock(thread_mutex_t* pMutex)
{
    if(pMutex)
    {
        m_mutex = pMutex;
        pthread_mutex_lock(m_mutex);
    }
    else
    {
        m_mutex = NULL;
    }
}

CAuthThreadLock::~CAuthThreadLock()
{
    if(m_mutex)
    {
    pthread_mutex_unlock(m_mutex);
    }
}
