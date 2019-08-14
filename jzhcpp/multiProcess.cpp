#include "multiProcess.h"
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <iostream>

struct Task
{
	int ID;
	std::string TaskInfo;

	// 其他信息略 ...

	Task() : ID(0) {}
};

void Process(const Task& task)
{
	// 测试
	std::cout << std::this_thread::get_id() << ", " <<task.TaskInfo << std::endl;
}

class MultiProcessImpl
{
public:
	MultiProcessImpl()
		: m_thread_quit(false)
	{
	}

	bool start(unsigned short thread_count) 
	{
		if (thread_count < 4) {
			thread_count = 4;
		}

		for (unsigned short i = 0; i < thread_count; ++i) {
			auto pthread = new std::thread(&MultiProcessImpl::do_process, this);
			m_threads.push_back(pthread);
		}

		return false;
	}

	void stop()
	{
		m_thread_quit = true;

		// <唤醒所有执行可执行的任务>
		m_task_wake.notify_all();

		for (auto p : m_threads) {
			p->join();
			delete p;
		}

		m_task_list.clear();
	}

	bool task_process(const Task& task)
	{
		std::unique_lock<std::mutex> lock(m_task_mutex);

		m_task_list.push_back(task);

		m_task_wake.notify_one();

		return true;
	}

private:
	void do_process()
	{
		std::unique_lock<std::mutex> lock(m_task_mutex);
		while (!m_thread_quit)
		{
			// <防止多个线程同时任务导致不能被唤醒>
			m_task_wake.wait_for(lock, std::chrono::milliseconds(25));

			if (m_thread_quit) {
				break;
			}

			// <无任务>
			if (m_task_list.empty()) {
				continue;
			}

			Task task = m_task_list.front();
			m_task_list.pop_front();

			// <取出任务进行处理,释放锁,其他线程可持有锁>
			m_task_mutex.unlock();

			// <处理任务>
			Process(task);

			// <加锁,继续循环>
			m_task_mutex.lock();
		}
	}

private:
	volatile bool m_thread_quit;
	
	std::vector<std::thread*> m_threads;
	std::condition_variable m_task_wake;
	std::mutex m_task_mutex;
	std::list<Task> m_task_list;
};


static MultiProcessImpl g_multiprocess_impl;

bool MultiProcess::start(unsigned short thread_count)
{
	return g_multiprocess_impl.start(thread_count);
}

void MultiProcess::stop()
{
	g_multiprocess_impl.stop();
}

bool MultiProcess::task_process(const std::string& task_info, TaskProc proc)
{
	Task task;
	task.TaskInfo = task_info;

	return g_multiprocess_impl.task_process(task);
}

