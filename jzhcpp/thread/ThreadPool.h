// ThreadLocal.h
// Definition of the ThreadLocal.
// Copyleft 2018, jzh just do it

#ifndef THREAD_THREADPOOL_H
#define THREAD_THREADPOOL_H
#include <vector>
#include "Mutex.h"
#include "Thread.h"

namespace yep {
namespace thread {

class Runnable;
class PooledThread;

// TODO: int to int32

// <no used thread will be removed>
class ThreadPool
{
public:
	// <线程空跑超过idletime将被删除>
	ThreadPool(int minCapacity = 2,
		int maxCapacity = 16,
		int idleTime = 60,
		int stackSize = 0);

	ThreadPool(const std::string& name,
		int minCapacity = 2,
		int maxCapacity = 16,
		int idleTime = 60,
		int stackSize = 0);

	~ThreadPool();

	const std::string& name() const { return name_; }

	// <n 负数减少>
	void addCapacity(int n);

	int capacity() const { Mutex::ScopedLock lock(mutex_); return maxCapacity_; }

	void setStackSize(int size) { stackSize_ = size; }

	int getStackSize() const { return stackSize_; };

	// <当前使用>
	int used() const;

	// <当前分配>
	int allocated() const;

	// <当前可用>
	int available() const;

	void start(Runnable& target);

	void start(Runnable& target, const std::string& name);

	void startWithPriority(Thread::Priority priority, Runnable& target);

	void startWithPriority(Thread::Priority priority, Runnable& target, const std::string& name);

	void stopAll();

	// <等待runabled 线程完成(不是所有)>
	void joinAll();

	// <移除不再使用的线程, 在start(), addCapacity(), joinAll() 方法中隐私调用>
	void collect();

	// <默认线程池,单例>
	static ThreadPool& defaultPool();
public:
	ThreadPool(const ThreadPool& pool) = delete;
	ThreadPool& operator = (const ThreadPool& pool) = delete;
protected:
	PooledThread* getThread();
	PooledThread* createThread();

	// <清理idle线程>
	void housekeep();
private:
	typedef std::vector<PooledThread*> ThreadVec;

	std::string name_;
	unsigned int minCapacity_;	// <容量>
	unsigned int maxCapacity_;
	int			idleTime_;		// <空闲时间>
	int			serial_;		// <序号>
	int			age_;
	int			stackSize_;

	ThreadVec	  threads_;
	mutable Mutex mutex_;
};

} // !namespace thread
} // !namespace yep

#endif // !THREAD_THREADPOOL_H