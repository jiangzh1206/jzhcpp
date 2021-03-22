#pragma once

#include <functional>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <vector>
#include <stack>
#include <set>

namespace JTimer {

using timer_id = std::size_t;
using handler_t = std::function<void(timer_id)>;
using clock = std::chrono::steady_clock;
using timestamp = std::chrono::time_point<clock>;
using duration = std::chrono::microseconds;

namespace detail {
	struct Event
	{
		timer_id	id;
		timestamp	start;		// 开始
		duration	period;		// 周期(循环）
		handler_t	handler;
		bool		valid;

		Event()
			: id(0), start(duration::zero()), period(duration::zero()), handler(nullptr), valid(false)
		{
		}

		template<typename Func>
		Event(timer_id id, timestamp start, duration period, Func&& handler)
			: id(id), start(start), period(period), handler(std::forward<Func>(handler)), valid(true)
		{
		}

		// 移动默认
		Event(Event &&r) = default;
		Event &operator=(Event &&ev) = default;
		
		// 拷贝复制删除 
		Event(const Event &r) = delete;
		Event &operator=(const Event &r) = delete;
	};

    // next 触发时间
    struct TimeEvent
    {
        timestamp next;
        timer_id ref;
    };

    // 放到set中排序
    inline bool operator<(const TimeEvent& lhs, const TimeEvent& rhs)
    {
        return lhs.next < rhs.next;
    }
} // namespace detail

class Timer
{
	using ScopedLock = std::unique_lock<std::mutex>;

    // timer thread
    std::thread m_worker;
    std::mutex m_mutex;
    std::condition_variable m_cv;

	// terminate thread
	std::atomic_bool			m_done;
	
	// all event
	std::vector<detail::Event>	m_events;
	
	// sorted queue, next timeout at ites top
	std::multiset<detail::TimeEvent> m_timeEvents;

    // 重复使用timer_id
    std::stack<ns_timer::timer_id> m_freeIds;

public:
	Timer()
	{
		m_done = false;
		m_worker = std::thread(std::bind(&Timer::run, this));
	}

public:
    static Timer* Ptr() 
    {
        static Timer timer;
        return &timer;
    }

public:

    ~Timer()
    {
        m_done.store(true);

		m_cv.notify_all();
		m_worker.join();

		// clear all event
		m_events.clear();
		while (!m_freeIds.empty()) {
			m_freeIds.pop();
		}
	}

	timer_id Add(const timestamp& when, handler_t&& handler, const duration& period = duration::zero())
	{
		ScopedLock lk(m_mutex);

		timer_id id = 0;
		// freeid 空创建
		if (m_freeIds.empty()) {
			id = m_events.size();	// id = event count
			detail::Event e(id, when, period, std::move(handler));
			m_events.push_back(std::move(e));
		} else {
			id = m_freeIds.top();
			m_freeIds.pop();
			detail::Event e(id, when, period, std::move(handler));
			m_events[id] = std::move(e);
		}
		m_timeEvents.insert(detail::TimeEvent{when, id});
		lk.unlock();

		m_cv.notify_all();

		return id;
	}

	template <class Rep, class Period>
	inline timer_id Add(const std::chrono::duration<Rep, Period> &when, handler_t &&handler, const duration &period = duration::zero())
	{
		// like std::chrono::seconds(), millionseconds
		return Add(clock::now() + std::chrono::duration_cast<std::chrono::microseconds>(when),
			std::move(handler), period);
	}

	inline timer_id Add(const uint64_t when, handler_t &&handler, const uint64_t period = 0)
	{
		return Add(duration(when), std::move(handler), duration(period));
	}

	bool Remove(timer_id id)
	{
		ScopedLock lk(m_mutex);
		if (m_events.size() == 0 || m_events.size() < id) {
			return false;
		}

		m_events[id].valid = false;
		auto it = std::find_if(m_timeEvents.begin(), m_timeEvents.end(), [&](const detail::TimeEvent& e){
			return e.ref == id;
		});

		if (it != m_timeEvents.end()) {
			m_freeIds.push(it->ref);
			m_timeEvents.erase(it);
		}
		lk.unlock();
		m_cv.notify_all();
		return true;
	}


private:
	void run()
	{
		ScopedLock lk(m_mutex);
		while (!m_done.load()) {
			if (m_timeEvents.empty()) {
				m_cv.wait(lk);
			} else {
				auto e = *m_timeEvents.begin();
				if (JTimer::clock::now() >= e.next) {
					m_timeEvents.erase(m_timeEvents.begin());

					// invoke handler
					lk.unlock();
					m_events[e.ref].handler(e.ref);
					lk.lock();

					if (m_events[e.ref].valid && m_events[e.ref].period.count() > 0) {
						// 周期性的timer，继续下一次
						e.next += m_events[e.ref].period;
						m_timeEvents.insert(e);
					} else {
						m_events[e.ref].valid = false;
						m_freeIds.push(e.ref);	// 复用(不删除)
					}
				} else {
					m_cv.wait_until(lk, e.next); // 等待到第一次触发
				}
			}
		}
	}
};


} // namespace NsTimer