// Event.h
// Definition of the Event.
// Copyleft 2018, jzh just do it


#ifndef THREAD_EVENT_H
#define THREAD_EVENT_H
#include "..\base\Basic.h"
namespace yep {

namespace thread
{

// <windows Event 封装>
class Event
{
public:
	// <默认自动复位>
	Event(bool autoReset = true);
	~Event();
public:
	Event(const Event&) = delete;
	Event& operator = (const Event&) = delete;

public:
	void set();
	// <无信号状态>
	void reset();

	void wait();
	bool wait(long milliseconds);

private:
	HANDLE event_;
};

inline Event::Event(bool autoReset)
{
	event_ = CreateEventW(NULL, autoReset ? TRUE : FALSE, FALSE, NULL);
	if (!event_)
	{
		throw std::exception("can not create event");
	}
}

inline Event::~Event()
{
	CloseHandle(event_);
}

inline void Event::set()
{
	if (!SetEvent(event_))
	{
		throw std::exception("cannot signal event");
	}
}

inline void Event::reset()
{
	if (!ResetEvent(event_))
	{
		throw std::exception("cannot reset event");
	}
}

inline void Event::wait()
{
	switch (WaitForSingleObject(event_, INFINITE))
	{
	case WAIT_OBJECT_0:
		return;
	default:
		throw std::exception("wait for event failed");
	}
}


inline bool Event::wait(long milliseconds)
{
	switch (WaitForSingleObject(event_, milliseconds + 1))
	{
	case WAIT_TIMEOUT:
		return false;
	case WAIT_OBJECT_0:
		return true;
	default:
		throw std::exception("wait for event failed");
	}
}

} // !namespace JAnt
} // !namespace yep

#endif // THREAD_EVENT_H