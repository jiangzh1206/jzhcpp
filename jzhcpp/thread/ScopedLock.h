// ScopedLock.h
// Definition of the ScopedLock.
// Copyleft 2018, jzh just do it

#ifndef THREAD_SCOPED_LOCK_H
#define THREAD_SCOPED_LOCK_H

namespace yep {
namespace thread{

template<typename T>
class ScopedLock
{
public:
	ScopedLock() = delete;
	ScopedLock(const ScopedLock&) = delete;
	ScopedLock& operator = (const ScopedLock&) = delete;

public:
	explicit ScopedLock(T& mutex) : mutex_(mutex)
	{
		mutex_.lock();
	}

	ScopedLock(T& mutex, long milliseconds) : mutex_(mutex)
	{
		mutex_.lock(milliseconds);
	}

	~ScopedLock() throw()
	{
		mutex_.unlock();
	}
private:
	T& mutex_;
};

// <with unlock method>
template<typename T>
class ScopedLockWithUnlock
{
public:
	ScopedLockWithUnlock() = delete;
	ScopedLockWithUnlock(const ScopedLockWithUnlock&) = delete;
	ScopedLockWithUnlock& operator = (const ScopedLockWithUnlock&) = delete;

public:
	explicit ScopedLockWithUnlock(T& mutex) : pmutex_(mutex)
	{
		pmutex_->lock();
	}

	ScopedLockWithUnlock(T& mutex, long milliseconds) : mutex_(&mutex)
	{
		pmutex_->lock(milliseconds);
	}

	~ScopedLockWithUnlock()
	{
		unlock();
	}

	void unlock()
	{
		if (pmutex_) {
			pmutex_->Unlock();
			pmutex_ = nullptr;
		}
	}

private:
	T* pmutex_;
};

} // !namespace thread
} // !namespace yep

#endif