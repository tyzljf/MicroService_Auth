mp_int32 CMPThread::InitMutex(thread_mutex_t* thread)
{
	mp_int32 ret = pthread_mutex_init(thread, NULL);
	if(ret != 0)
	{
		return MP_FAILED;
	}

	return MP_SUCCESS;
}

mp_int32 CMPThread::DestoryMutex(thread_mutex_t* thread)
{
	mp_int32 ret = pthread_mutex_destroy(thread);
	if(ret != 0)
	{
		return MP_FAILED;
	}

	return MP_SUCCESS;
}

mp_int32 CMPThread::CreateThread(thread_t* threadId, void*(*routine)(void *), void* arg)
{
	mp_int32 ret = pthread_create(threadId, NULL, routine, arg);
	if(ret != 0)
	{
		return MP_FAILED;
	}

	return MP_SUCCESS;
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

