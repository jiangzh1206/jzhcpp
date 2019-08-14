// Mutex.h
// Definition of the Mutex.
// Copyleft 2018, jzh just do it

#ifndef THREAD_MUTEX_H
#define THREAD_MUTEX_H

#include "MutexImpl.h"
#include "ScopedLock.h"


namespace yep {

namespace thread{

// <可递归>
// <可用于 std::condition_variable_any, 实现lock,unlock>
class Mutex : private MutexImpl{
public:
	Mutex(const Mutex&) = delete;
	Mutex& operator = (const Mutex&) = delete;

public:
	typedef ScopedLock<Mutex> ScopedLock;

	Mutex() {}
	~Mutex() {}

	void lock();

	// <阻塞毫秒数>
	void lock(long milliseconds);

	bool tryLock();
	bool tryLock(long milliseconds);

	void unlock();
};


// <empty mutex >
class NullMutex
	/// A NullMutex is an empty mutex implementation
	/// which performs no locking at all. Useful in policy driven design
	/// where the type of mutex used can be now a template parameter allowing the user to switch
	/// between thread-safe and not thread-safe depending on his need
	/// Works with the ScopedLock class
{
public:
	typedef yep::thread::ScopedLock<NullMutex> ScopedLock;

	NullMutex() {}

	~NullMutex() {}

	void lock()
	{
		// <Does nothing>.
	}

	void lock(long)
	{
		// <Does nothing>
	}

	bool tryLock()
	{
		// <Does nothing and always returns true>
		return true;
	}

	bool tryLock(long)
	{
		// <Does nothing and always returns true>
		return true;
	}

	void unlock() { }
};

inline void Mutex::lock()
{
	lockImpl();
}

inline void Mutex::lock(long milliseconds)
{
	if (!tryLockImpl(milliseconds)) {
		throw std::exception("try lock mutex timeout");
	}
}

inline bool Mutex::tryLock()
{
	return tryLockImpl();
}

inline bool Mutex::tryLock(long milliseconds)
{
	return tryLockImpl(milliseconds);
}

inline void Mutex::unlock()
{
	unlockImpl();
}

} // !namespace thread

}// ! namespace yep

#endif // !THREAD_MUTEX_H