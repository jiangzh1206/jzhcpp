#include "ThreadLocal.h"
#include "Thread.h"
#include "SingletonHolder.h"

namespace yep {
namespace thread {

ThreadLocalStorage::ThreadLocalStorage()
{
}

ThreadLocalStorage::~ThreadLocalStorage()
{
	for (auto& tls : map_) {
		delete tls.second;
	}
}

TLSAbstractSlot*& ThreadLocalStorage::get(const void* key)
{
	auto it = map_.find(key);
	if (it == map_.end()) {
		// <²åÈë·µ»Ømap<iterator, bool>>
		return map_.insert(TLSMap::value_type(key, reinterpret_cast<TLSAbstractSlot*>(0))).first->second;
	} else {
		return it->second;
	}
}

namespace
{
	static SingletonHolder<ThreadLocalStorage> sh;
}

ThreadLocalStorage& ThreadLocalStorage::current()
{
	Thread* pThread = Thread::current();
	if (pThread) {
		return pThread->tls();
	} else {
		return *sh.get();
	}
}

void ThreadLocalStorage::clear()
{
	Thread* pThread = Thread::current();
	if (pThread) {
		pThread->clearTLS();
	}
}

} // !namespace thread
} // !namespace yep