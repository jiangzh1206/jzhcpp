#include "Thread.h"
#include <sstream>
#include <atomic>
#include <process.h>
#include "ThreadLocal.h"

namespace yep {
namespace thread {
		
// <static init>
Thread::CurrentThreadHolder Thread::currentThreadHolder_;

Thread::~Thread()
{
	delete pTLS_;
}

Thread::Thread()
	: threadId_(0)
	, thread_(0)
	, priority_(THREAD_PRIORITY_NORMAL)
	, stackSize_(0)
	, id_(uniqeId())
	, name_(makeName())
	, pTLS_(0)
	, event_(true)
{
	;
}

Thread::Thread(const std::string& name)
	: threadId_(0)
	, thread_(0)
	, priority_(THREAD_PRIORITY_NORMAL)
	, stackSize_(0)
	, id_(uniqeId())
	, name_(name)
	, pTLS_(0)
	, event_(true)
{
	;
}

void Thread::setPriority(Priority priority)
{
	if (priority_ != priority) {
		priority_ = priority;
		if (thread_) {
			if (SetThreadPriority(thread_, priority_) == 0)
			{
				throw std::exception("cannot set thread priority");
			}
		}
	}
}

void Thread::join()
{
	if (!thread_) {
		return;
	}

	switch (WaitForSingleObject(thread_, INFINITE))
	{
	case WAIT_OBJECT_0:
		closeThread();
		return;
	default:
		throw std::exception("cannot join thread");
	}
}

bool Thread::join(long milliseconds)
{
	if (!thread_) {
		return true;
	}
	switch (WaitForSingleObject(thread_, milliseconds))
	{
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		closeThread();
		return true;
	default:
		throw std::exception("cannot join thread");
	}
}

bool Thread::trySleep(long milliseconds)
{
	Thread* pT = Thread::current();
	if (!pT) {
		std::abort();
	}
	return !(pT->event_.wait(milliseconds));
}

void Thread::startImpl(Runnable* ptarget)
{
	if (isRunning()) {
		throw std::exception("thread is runnint");
	}
	prunnable_.reset(ptarget);
	createThread(threadEntry, this);
}

void Thread::createThread(Entry entry, void* data)
{
	unsigned threadId;
	thread_ = (HANDLE)_beginthreadex(NULL, stackSize_, entry, this, 0, &threadId);
	threadId_ = static_cast<DWORD>(threadId);

	if (!thread_)
		throw std::exception("cannot create thread");
	if (priority_ != PRIO_NORMAL && !SetThreadPriority(thread_, priority_))
		throw std::exception("cannot set thread priority");
}

ThreadLocalStorage& Thread::tls()
{
	// <null will create>
	if (!pTLS_)
	{
		pTLS_ = new ThreadLocalStorage;
	}
	return *pTLS_;
}

void Thread::clearTLS()
{
	if (pTLS_) {
		delete pTLS_;
		pTLS_ = nullptr;
	}
}

bool Thread::isRunning() const
{
	if (thread_) {
		DWORD ec;
		return GetExitCodeThread(thread_, &ec) && ec == STILL_ACTIVE;
	}

	return false;
}

void Thread::closeThread()
{
	if (thread_) {
		CloseHandle(thread_);
		thread_ = nullptr;
	}
}

int Thread::uniqeId()
{
	static std::atomic<int> id = 0;
	return ++id;
}

std::string Thread::makeName()
{
	std::stringstream name;
	name << "#" << id_;
	return name.str();
}

unsigned int __stdcall Thread::threadEntry(void* pthread)
{
	// <set TLS>
	currentThreadHolder_.set(reinterpret_cast<Thread*>(pthread));

	// <调用线程执行函数>
	try
	{
		reinterpret_cast<Thread*>(pthread)->prunnable_->run();
	}
	catch (...)
	{
		// thread proccess error;
	}

	return 0;
}


} // !namespace thread
} // !namespace yep