// SingletonHolder.h
// Definition of the SingletonHolder.
// Copyleft 2018, jzh just do it

#ifndef THREAD_SINGLETONHOLDER_H
#define THREAD_SINGLETONHOLDER_H
#include "Mutex.h"


namespace yep {
namespace thread{

// <c++11 保证函数内部静态变量线程安全>
template<class T>
class TSingleton
{
public:
	TSingleton() = delete;

	static T& get()
	{
		static T p;
		return p;
	}
};

template <class T>
class SingletonHolder
{
public:
	SingletonHolder() : p_(0) { }

	~SingletonHolder() { delete p_; }

	T* get()
	{
		Mutex::ScopedLock lock(m_);
		if (!p_) p_ = new T;
		return p_;
	}

	void reset()
	{
		Mutex::ScopedLock lock(m_);
		delete p_;
		p_ = 0;
	}

private:
	T* p_;
	Mutex m_;
};

} // !namespace thread
} // !namespace yep

#endif // !THREAD_SINGLETONHOLDER_H