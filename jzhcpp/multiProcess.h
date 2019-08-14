#pragma once

#include <functional>
#include <string>

class MultiProcess
{
	typedef std::function<void(bool)> TaskProc;

public:
	static bool start(unsigned short thread_count);

	static void stop();

	static bool task_process(const std::string& task_info, TaskProc proc);

private:
	MultiProcess();
};