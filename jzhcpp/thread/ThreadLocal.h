// ThreadLocal.h
// Definition of the ThreadLocal.
// Copyleft 2018, jzh just do it

#ifndef THREAD_THREADLOCAL_H
#define THREAD_THREADLOCAL_H
#include <map>

namespace yep {
namespace thread{
// <TLSSlot 基类, 返回对象>
class TLSAbstractSlot
{
public:
	TLSAbstractSlot() {}
	virtual ~TLSAbstractSlot() {}
};

// <tls对象包装>
template<class T>
class TLSSlot : public TLSAbstractSlot
{
public:
	TLSSlot() : value_(){}
	~TLSSlot() {}
	T& value(){ return value_; }
public:
	TLSSlot(const TLSSlot&) = delete;
	TLSSlot& operator=(const TLSSlot&) = delete;
private:
	T value_;
};

// <local storage for thread>
class ThreadLocalStorage
{
public:
	ThreadLocalStorage();
	~ThreadLocalStorage();

	TLSAbstractSlot*& get(const void* key);
	// <can main thread>
	static ThreadLocalStorage& current();
	static void clear();
private:
	typedef std::map<const void*, TLSAbstractSlot*> TLSMap;
	TLSMap map_;

	friend class Thread;
};


// <manamgs TLS object The underlying object will
// be created when it is referenced for the first
// time>
template<class T>
class ThreadLocal
{
public:
	ThreadLocal() {}
	~ThreadLocal() {}

	T* operator ->()
	{
		return &get();
	}

	T& operator *()
	{
		return get();
	}

	T& get()
	{
		TLSAbstractSlot*& p = ThreadLocalStorage::current().get(this);
		if (!p) {
			p = new TLSSlot<T>;
		}
		return static_cast<TLSSlot<T>*>(p)->value();
	}

public:
	ThreadLocal(const ThreadLocal&) = delete;
	ThreadLocal& operator = (const ThreadLocal&) = delete;
};


} // !namespace thread
} // !namespace yep

#endif // !THREAD_THREADLOCAL_H