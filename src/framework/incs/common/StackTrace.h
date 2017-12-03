#ifndef STACK_TRACE_H_
#define STACK_TRACE_H_

#include <string>

class StackTracer
{
public:
	StackTracer();
	virtual ~StackTracer();

private:
	void installSignalHandler();
	void uninstallSignalHandler();

	static int signalHandler(int signum, void *siginfo, void *ucontext);
	static void generateStackTrace(std::string &stackTrace, int startFrameOffset, int numFrames);
	static void printStackTrace(const std::string& stackTrace);
	static void safeAbort();
};

#endif

