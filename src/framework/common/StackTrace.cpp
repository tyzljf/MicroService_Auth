/*****************************************************
	StackTrace.cpp --- 进程异常退出时，捕获堆栈信息
	Author: tanyouzhang
	Date: 2017/05/04
/****************************************************/

#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "StackTracer.h"


#define STACK_TRACE_FRAME_NUMBER 16 // the number of the frame of the stack trace
#define STACK_TRACE_MAX_FRAME    32 // the max number of the frame of the stack trace
#define STACK_TRACE_INITIAL_FRAME 3 // the initial number of the frame of the stack trace

typedef void(*signal_handler_t)(int, siginfo_t *, void *);

using namespace std;
namespace hcp
{
	static void installSignalHandler(int signum, signal_handler_t handler)
	{
		struct sigaction sa;

		memset(&sa, 0, sizeof(struct sigaction));
		sa.sa_sigaction = handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_ONSTACK;
		(void)sigaction(signum, &sa, NULL);
	}

	static void uninstallSignalHandler(int signum)
	{
		struct sigaction sa;
		(void)sigaction(signum, NULL, &sa);
		sa.sa_handler = SIG_DFL;
		(void)sigaction(signum, &sa, NULL);
	}
}

StackTracer::StackTracer()
{
	installSignalHandler();
}

StackTracer::~StackTracer()
{
	uninstallSignalHandler();
}

void StackTracer::installSignalHandler()
{
	hcp::installSignalHandler(SIGABRT,(signal_handler_t)StackTracer::signalHandler);
	hcp::installSignalHandler(SIGFPE,(signal_handler_t)StackTracer::signalHandler);
	hcp::installSignalHandler(SIGSEGV,(signal_handler_t)StackTracer::signalHandler);
}

void StackTracer::uninstallSignalHandler()
{
	hcp::uninstallSignalHandler(SIGABRT);
	hcp::uninstallSignalHandler(SIGFPE);
	hcp::uninstallSignalHandler(SIGSEGV);
}

int StackTracer::signalHandler(int signum, void *siginfo,void * ucontext)
{
	(void)siginfo; // unused parameters
	(void)ucontext; // unused parameters

	if((signum != SIGABRT) && (signum != SIGFPE) && (signum != SIGSEGV))
	{
		return 0;
	}

	std::string stackTrace;
	generateStackTrace(stackTrace, 0, STACK_TRACE_FRAME_NUMBER);
	printStackTrace(stackTrace);
	safeAbort();
}

void StackTracer::generateStackTrace(string & stackTrace,int startFrameOffset,int numFrames)
{
	void *stack[STACK_TRACE_MAX_FRAME];
	char **symbols = NULL;
	
	int startFrame = STACK_TRACE_INITIAL_FRAME>startFrameOffset?STACK_TRACE_INITIAL_FRAME:startFrameOffset;

	std::stringstream ss;
	int stackSize = ::backtrace(stack, STACK_TRACE_MAX_FRAME);
	if(stackSize > 0)
	{
		symbols = ::backtrace_symbols(stack, stackSize);
		for(int i = 0; i < stackSize && numFrames > 0; i++, numFrames--)
			ss << symbols[i] << "\n";
		::free(symbols);
	}
	else
	{
		ss << "<unable to get stack trace information>";
	}

	stackTrace = ss.str();
}

void StackTracer::printStackTrace(const string & stackTrace)
{
	void *array[STACK_TRACE_MAX_FRAME];
	char **strings = NULL;
	size_t i;
	
	int stackSize = backtrace(array, STACK_TRACE_MAX_FRAME);
	strings = backtrace_symbols(array, stackSize);

	if(NULL == strings)
	{
		return;
	}

	for(i = 0; i < stackSize; i++)
	{
		if(NULL != strings[i])
		{
			cout << strings[i] << endl;
		}
	}

	free(strings);
}

void StackTracer::safeAbort()
{
	hcp::uninstallSignalHandler(SIGABRT);
	abort();
}
