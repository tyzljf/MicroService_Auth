#ifndef __TYPES_H
#define __TYPES_H

#include <pthread.h>
#include <iostream>
#include <time.h>

typedef int 				mp_int32;
typedef unsigned int    	mp_uint32;
typedef long int 			mp_int64;
typedef unsigned long int 	mp_uint64;

typedef char 				mp_char;
typedef std::string 		mp_string;

typedef time_t				mp_time;
typedef struct tm           mp_tm;

typedef pthread_mutex_t 	thread_mutex_t;
typedef pthread_t			thread_t;


#define MP_SUCCESS			 0
#define MP_FAILED			-1


#endif