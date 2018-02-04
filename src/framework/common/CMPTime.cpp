#include "CMPTime.h"


void CMPTime::DoSleep(mp_uint32 ms)
{
	struct timeval time;
	time.tv_sec = ms / 1000;
  	time.tv_usec = (ms* 1000) % 1000000;
   	select(0,NULL,NULL,NULL,&time);
}


mp_int32 CMPTime::Time(mp_time *pTime)
{
	if(NULL == pTime)
	{
		return MP_FAILED;
	}

	int ret = time(pTime);
	if(ret < 0)
	{
		return MP_FAILED;
	}

	return MP_SUCCESS;
}

mp_int32 CMPTime::LocalTimeR(const mp_time* now, mp_tm* result)
{
	if(NULL == now || NULL == result)
	{
		return MP_FAILED;
	}

	mp_tm *res = localtime_r(now, result);
	if(NULL == res)
	{
		return MP_FAILED;
	}

	return MP_SUCCESS;
}



