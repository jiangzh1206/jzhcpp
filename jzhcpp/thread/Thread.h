// Thread.h
// Definition of the Thread.
// Copyleft 2018, jzh just do it

#ifndef THREAD_THREAD_H
#define THREAD_THREAD_H
#include <string>
#include <memory>
#include <functional>
#include "..\base\Basic.h"
#include "Event.h"
#include "Mutex.h"

namespace yep {
namespace thread {

class Runnable
{
public:
	Runnable() {};
	virtual ~Runnable() {};

	virtual void run() = 0;
};

class RunnableHolder : public Runnable
{
public:
	RunnableHolder(Runnable& target) : target_(target) {}
	~RunnableHolder(){}
	void run(){ target_.run(); }
private:
	Runnable& target_;
};

typedef void(*Callable)(void*);

class CallableHolder : public Runnable
{
public:
	CallableHolder(Callable callable, void* pData) : callable_(callable), pData_(pData)
	{
	}
	~CallableHolder() { }
	void run() { callable_(pData_); }

private:
	Callable callable_;
	void* pData_;
};


// <function callback>
template<class Functor>
class FunctorRunnable : public Runnable
{
public:
	FunctorRunnable(const Functor& functor) : functor_(functor){}
	~FunctorRunnable(){}
	void run() { functor_(); }

private:
	Functor functor_;
};


class ThreadLocalStorage;


// <windows 线程封装>
class Thread
{
public:
	typedef DWORD TID;
	typedef unsigned int(__stdcall *Entry)(void*);

	// <优先级>
	enum Priority
		/// Thread priorities.
	{
		PRIO_LOWEST  = THREAD_PRIORITY_LOWEST,		 /// The lowest thread priority.
		PRIO_LOW     = THREAD_PRIORITY_BELOW_NORMAL, /// A lower than normal thread priority.
		PRIO_NORMAL  = THREAD_PRIORITY_NORMAL,		 /// The normal thread priority.
		PRIO_HIGH    = THREAD_PRIORITY_ABOVE_NORMAL, /// A higher than normal thread priority.
		PRIO_HIGHEST = THREAD_PRIORITY_HIGHEST		 /// The highest thread priority.
	};

	// <策略>
	enum Policy
	{
		POLICY_DEFAULT = 0
	};

public:
	Thread();
	Thread(const std::string& name);
	~Thread();

public:
	// <cannot copy>
	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

public:
	TID tid()const { return threadId_; };
	
	int32 id()const { return id_; }

	std::string name()const { return name_; }

	Priority priority()const { return Priority(priority_); }

	int32 stackSize() const { return stackSize_; }

	void setName(const std::string& name) { Mutex::ScopedLock lock(mutex_); name_ = name; }

	void setStackSize(int size) { stackSize_ = size; }

	void setPriority(Priority priority);

public:
	void start(Runnable& target) { startImpl(new RunnableHolder(target)); }
	void start(Callable target, void* data = nullptr) { startImpl(new CallableHolder(target, data)); }

	// <仿函数, lambda>
	template<class Functor>
	void startFunc(Functor Fn)
	{
		startImpl(new FunctorRunnable<Functor>(Fn));
	}

	void join();
	bool join(long milliseconds);
	bool tryJoin(long milliseconds) { return join(milliseconds); }
	void wakeUp(){ event_.set(); }

public:
	static void sleep(long milliseconds) { Sleep(DWORD(milliseconds)); }

	// <false wakeup唤醒, true 等待时间到>
	static bool trySleep(long milliseconds);
	static void yield() { Sleep(0); }
	static Thread* current() { return currentThreadHolder_.get(); }
	static TID currentTid() { return GetCurrentThreadId(); }
	bool isRunning()const;

private:
	void startImpl(Runnable* ptarget);
	void createThread(Entry entry, void* data);

private:
	ThreadLocalStorage& tls();
	void clearTLS();

	static int uniqeId();
	std::string makeName();

	void closeThread();

	static unsigned int __stdcall threadEntry(void* pthread);
private:
	// <线程本地存储,创建后各自不同>
	class CurrentThreadHolder
	{
	public:
		CurrentThreadHolder() :slot_(TlsAlloc())
		{
			if (slot_ == TLS_OUT_OF_INDEXES)
			{
				throw std::exception("cannot alloc thread tls key");
			}
		}
		~CurrentThreadHolder() { TlsFree(slot_); }

		// <当前线程中调用>
		Thread* get()const
		{
			return reinterpret_cast<Thread*>(TlsGetValue(slot_));
		}

		void set(Thread* pthread)
		{
			TlsSetValue(slot_, pthread);
		}
	private:
		// <TLS 数组索引>
		DWORD slot_;
	};

private:
	// <线程id>
	TID					threadId_;
	HANDLE				thread_;
	int32				priority_;
	int32				stackSize_;

	int32				id_;
	std::string			name_;
	ThreadLocalStorage* pTLS_;
	Event				event_;
	mutable Mutex		mutex_;

	// <线程执行>
	std::shared_ptr<Runnable> prunnable_;

	static CurrentThreadHolder currentThreadHolder_;

	friend class ThreadLocalStorage;
};

} // !namespace thread
} // !namespace yep
#endif // !THREAD_THREAD_H