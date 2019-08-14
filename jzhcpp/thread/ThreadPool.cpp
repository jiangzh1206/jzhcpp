#include "ThreadPool.h"
#include <assert.h>
#include "ThreadLocal.h"
#include <sstream>

namespace yep {
namespace thread{

// <Thread wrapper>
class PooledThread : public Runnable
{
public:
	PooledThread(const std::string& name, int stackSize = 0);
	~PooledThread(){};

	void start();
	void start(Thread::Priority priority, Runnable& target);
	void start(Thread::Priority priority, Runnable& target, const std::string& name);
	bool idel();
	int idelTime();
	void join();
	// <激活,完成状态置空>
	void activate();
	void release();
	void run();
private:
	volatile bool			idle_;
	volatile std::time_t	idleTime_;
	Runnable*				pTarget_;
	std::string				name_;

	Thread  thread_;
	Mutex   mutex_;
	Event	targetReady_;
	Event	targetCompleted_;
	Event	started_;
};

PooledThread::PooledThread(const std::string& name, int stackSize /*= 0*/)
	: idle_(true)
	, idleTime_(0)
	, pTarget_(0)
	, name_(name)
	, thread_(name)
	, targetCompleted_(false)
{
	assert(stackSize >= 0);
	idleTime_ = std::time(0);
}

void PooledThread::start()
{
	thread_.start(*this);
	started_.wait();
}

void PooledThread::start(Thread::Priority priority, Runnable& target)
{
	Mutex::ScopedLock lock(mutex_);

	assert(pTarget_ == 0);

	pTarget_ = &target;
	thread_.setPriority(priority);

	// <target 设置完成>
	targetReady_.set();
}

void PooledThread::start(Thread::Priority priority, Runnable& target, const std::string& name)
{
	Mutex::ScopedLock lock(mutex_);

	assert(pTarget_ == 0);

	std::string fullName(name);
	if (name.empty())
	{
		fullName = name_;
	}
	else
	{
		fullName.append(" (");
		fullName.append(name_);
		fullName.append(")");
	}
	thread_.setName(fullName);

	pTarget_ = &target;
	thread_.setPriority(priority);

	// <target 设置完成>
	targetReady_.set();
}

bool PooledThread::idel()
{
	Mutex::ScopedLock lock(mutex_);
	return idle_;
}

int PooledThread::idelTime()
{
	Mutex::ScopedLock lock(mutex_);
	// <当前-构造时>
	return int(time(0) - idleTime_);
}

void PooledThread::join()
{
	mutex_.lock();
	Runnable* p = pTarget_;
	mutex_.unlock();

	if (p)
	{
		targetCompleted_.wait();
	}
}

void PooledThread::activate()
{
	Mutex::ScopedLock lock(mutex_);
	assert(idle_);
	idle_ = false;

	targetCompleted_.reset();
}

void PooledThread::release()
{
	const long JOIN_TIMEOUT = 10000;

	mutex_.lock();
	pTarget_ = 0;
	mutex_.unlock();
	// In case of a statically allocated thread pool (such
	// as the default thread pool), Windows may have already
	// terminated the thread before we got here.
	if (thread_.isRunning())
		targetReady_.set();

	if (thread_.tryJoin(JOIN_TIMEOUT))
	{
		delete this;
	}
}

void PooledThread::run()
{
	started_.set();
	for (;;)
	{
		// <等待target ready信号>
		targetReady_.wait();
		mutex_.lock();
		if (pTarget_) {// a NULL target means kill yourself
			Runnable* p = pTarget_;
			mutex_.unlock();
			try {
				p->run();
			}
			catch (...) {
				// run error
				assert(false);
			}
			
			Mutex::ScopedLock lock(mutex_);
			pTarget_ = 0;
			idleTime_ = time(NULL);
			idle_ = true;
			// <cmopleted>
			targetCompleted_.set();
			ThreadLocalStorage::clear();
			thread_.setName(name_);
			thread_.setPriority(Thread::PRIO_NORMAL);
		} else {
			mutex_.unlock();
			break;
		}
	}
}

ThreadPool::ThreadPool(int minCapacity /*= 2*/, 
	int maxCapacity /*= 16*/, 
	int idleTime /*= 60*/, 
	int stackSize /*= 0*/)
	: minCapacity_(minCapacity)
	, maxCapacity_(maxCapacity)
	, idleTime_(idleTime)
	, serial_(0)
	, age_(0)
	, stackSize_(stackSize)
{
	assert(minCapacity > 1 && maxCapacity >= minCapacity && idleTime > 0);

	for (unsigned int i = 0; i < minCapacity_; ++i)
	{
		PooledThread* pthread = createThread();
		threads_.push_back(pthread);
		pthread->start();
	}
}

ThreadPool::ThreadPool(const std::string& name, 
	int minCapacity /*= 2*/, 
	int maxCapacity /*= 16*/, 
	int idleTime /*= 60*/, 
	int stackSize /*= 0*/)
	: name_(name)
	, minCapacity_(minCapacity)
	, maxCapacity_(maxCapacity)
	, idleTime_(idleTime)
	, serial_(0)
	, age_(0)
	, stackSize_(stackSize)
{
	assert(minCapacity > 1 && maxCapacity >= minCapacity && idleTime > 0);

	for (unsigned int i = 0; i < minCapacity_; ++i)
	{
		PooledThread* pthread = createThread();
		threads_.push_back(pthread);
		pthread->start();
	}
}

ThreadPool::~ThreadPool()
{
	try
	{
		stopAll();
	}
	catch (...)
	{
		// unexcept
		assert(false);
	}
}

void ThreadPool::addCapacity(int n)
{
	Mutex::ScopedLock lock(mutex_);

	assert(maxCapacity_ >= minCapacity_);

	maxCapacity_ += n;

	// <重新清理等待线程>
	housekeep();
}

int ThreadPool::used() const
{
	Mutex::ScopedLock lock(mutex_);

	int n = 0;
	for (auto& t : threads_)
	{
		if (!t->idel()){
			++n;
		}
	}
	return n;
}

int ThreadPool::allocated() const
{
	Mutex::ScopedLock lock(mutex_);
	return int(threads_.size());
}

int ThreadPool::available() const
{
	Mutex::ScopedLock lock(mutex_);

	int n = 0;
	for (auto& t : threads_) {
		if (t->idel()){
			++n;
		}
	}
	return (int)(n + maxCapacity_ - threads_.size());
}

void ThreadPool::start(Runnable& target)
{
	getThread()->start(Thread::PRIO_NORMAL, target);
}

void ThreadPool::start(Runnable& target, const std::string& name)
{
	getThread()->start(Thread::PRIO_NORMAL, target, name);
}

void ThreadPool::startWithPriority(Thread::Priority priority, Runnable& target)
{
	getThread()->start(priority, target);
}

void ThreadPool::startWithPriority(Thread::Priority priority, Runnable& target, const std::string& name)
{
	getThread()->start(priority, target, name);
}

void ThreadPool::stopAll()
{
	Mutex::ScopedLock lock(mutex_);
	for (auto& t : threads_) {
		t->release();
	}
	threads_.clear();
}

void ThreadPool::joinAll()
{
	Mutex::ScopedLock lock(mutex_);
	for (auto& t : threads_) {
		t->join();
	}
	housekeep();
}

void ThreadPool::collect()
{
	Mutex::ScopedLock lock(mutex_);
	housekeep();
}

PooledThread* ThreadPool::getThread()
{
	Mutex::ScopedLock lock(mutex_);
	if (++age_ == 32) {
		housekeep();
	}

	PooledThread* pthread = nullptr;

	// <查找空闲>
	for (auto t : threads_) {
		if (t->idel())
		{
			pthread = t;
			break;
		}
	}

	// <无空闲>
	if (!pthread)
	{
		// <创建新的>
		if (threads_.size() < maxCapacity_) {
			pthread = createThread();
			try 
			{
				pthread->start();
				threads_.push_back(pthread);
			}
			catch (...)
			{
				delete pthread;
				assert(false);
				throw;
			}
		} else {
			// <超出capacity>
			assert(false);
		}
	}

	pthread->activate();
	return pthread;
}

PooledThread* ThreadPool::createThread()
{
	std::ostringstream name;
	name << name_ << "[#" << ++serial_ << "]";
	return new PooledThread(name.str(), stackSize_);
}

void ThreadPool::housekeep()
{
	age_ = 0;
	if (threads_.size() < minCapacity_) {
		// <不清理>
		return;
	}

	ThreadVec idleThreads;		// <空闲>
	ThreadVec expiredThreads;	// <超出idle时间的>
	ThreadVec activeThreads;	// <活动的>
	idleThreads.reserve(threads_.size());
	activeThreads.reserve(threads_.size());

	// <计算空闲和超出空闲的>
	for (auto& t : threads_) {
		if (t->idel()) {
			if (t->idelTime() < idleTime_) {
				idleThreads.push_back(t);
			} else {
				expiredThreads.push_back(t);
			}
		}
	}

	// <计算限制线程数>
	unsigned int n = activeThreads.size();
	unsigned int limit = idleThreads.size() + n;
	if (limit < minCapacity_) limit = minCapacity_;

	// <所有空闲的>
	idleThreads.insert(idleThreads.end(), expiredThreads.begin(), expiredThreads.end());
	threads_.clear();

	// <移除超过限制的>
	for (auto& t : idleThreads) {
		if (n < limit) {
			threads_.push_back(t);
			++n;
		}
		else t->release();
	}

	// <合并活动的>
	threads_.insert(threads_.end(), activeThreads.begin(), activeThreads.end());
}

class ThreadPoolSingletonHolder
{
public:
	ThreadPoolSingletonHolder()
	{
		pPool_ = 0;
	}
	~ThreadPoolSingletonHolder()
	{
		delete pPool_;
	}
	ThreadPool* pool()
	{
		Mutex::ScopedLock lock(mutex_);

		if (!pPool_) {
			pPool_ = new ThreadPool("default");
		}
		return pPool_;
	}

private:
	ThreadPool* pPool_;
	Mutex		mutex_;
};

namespace{
	static ThreadPoolSingletonHolder tpsh;
}

ThreadPool& ThreadPool::defaultPool()
{
	return *tpsh.pool();
}

} // !namespace thread
} // !namespace yep