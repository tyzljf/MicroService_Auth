#ifndef __CTHREAD_H
#define __CTHREAD_H

#include "Types.h"

class CThread
{
public:
    static int32_t InitLock(thread_mutex_t* thread);
    static int32_t DestoryLock(thread_mutex_t* thread);
    static int32_t CreateThread(thread_t* threadId, void*(*routine)(void *), void* arg);
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



