#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <mutex>
#include <utility>
#include <type_traits>
#include <ctime>
#include <cassert>
#include <chrono>
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <iomanip>
#include <filesystem>
#include <string_view>

namespace Logging {

class LogPolicyInterface
{
public:
	virtual void OpenOstream(const std::string& name) = 0;
	virtual void CloseOstream() = 0;
	virtual void Write(const std::string& msg) = 0;
};

class FileLogPolicy : public LogPolicyInterface
{
	std::ofstream mStream;

public:
	FileLogPolicy() : mStream() {}
	~FileLogPolicy() {}

	void OpenOstream(const std::string& name) override
	{
		mStream.open(name.c_str(), std::ios_base::binary | std::ios_base::out);
		assert(mStream.is_open() == true);
		mStream.precision(10);
	}
	
	void CloseOstream() override
	{
		if (mStream.is_open()) {
			mStream.close();
		}
	}

	void Write(const std::string& msg) override
	{
		mStream << msg << std::endl << std::flush;
	}
};

class ConsolePolicy : public LogPolicyInterface
{
public:
	ConsolePolicy() {}
	~ConsolePolicy() {}

	void OpenOstream(const std::string& name) override
	{
	}

	void CloseOstream() override
	{
	}

	void Write(const std::string& msg) override
	{
		std::cout << msg << std::endl << std::flush;
	}
};

enum SeverityType
{
	debug = 1,
	error,
	warning,
};
static std::map<SeverityType, std::string> gSeverityMap = {
	{SeverityType::debug, "Debug"},
	{SeverityType::error, "Error"},
	{SeverityType::warning, "Warn"}
};

template<typename Policy>
class Logger;

template<typename Policy>
void LoggingThread(Logger<Policy>* logger)
{
	std::unique_lock<std::timed_mutex> lk{logger->mWriteMutex, std::defer_lock};
	do {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (logger->mLogBuffer.size()) {
			lk.lock();
			for (auto& item : logger->mLogBuffer) {
				logger->mPolicy.Write(item);
			}
			logger->mLogBuffer.clear();
			lk.unlock();
		}
	} while (logger->mIsRunning.test_and_set() || logger->mLogBuffer.size());

	logger->mPolicy.Write("terminating the log thread...");
}

template<typename Policy>
class Logger
{
public:
	template<typename LOGPolicy>
	friend void LoggingThread(Logger<LOGPolicy>* logger);

	Logger(const std::string& name, bool console = false) {
		mPolicy.OpenOstream(name);

		mIsRunning.test_and_set();
		mLogThread = std::move(std::thread(LoggingThread<Policy>, this));
	}

	~Logger()
	{
		terminateLogger();
		mPolicy.Write("Logger terminate...");
		mPolicy.CloseOstream();
	}

	template<SeverityType severity, typename... Args>
	void Print(Args... args)
	{
		std::stringstream logStream;
		printIMPL(std::move(logStream), args...);
	}

private:
	void printIMPL(std::stringstream&& logStream)
	{
		std::lock_guard< std::timed_mutex > lock(mWriteMutex);
		mLogBuffer.push_back(logStream.str());
	}

	template<typename First, typename... Args>
	void printIMPL(std::stringstream&& logStream, First param, Args... args)
	{
		logStream << param;
		printIMPL(std::forward<std::stringstream>(logStream), std::move(args)...);
	}

	void terminateLogger()
	{
		mIsRunning.clear();
		mLogThread.join();
	}

private:
	Policy											mPolicy;
	std::timed_mutex								mWriteMutex;

	std::vector<std::string>						mLogBuffer;
	std::thread										mLogThread;
	std::atomic_flag								mIsRunning = ATOMIC_FLAG_INIT;
};

#ifndef LOG_CONSOLE
static Logging::Logger<Logging::FileLogPolicy> gLogger("excution.log");
#else
static Logging::Logger<Logging::ConsolePolicy> gLogger("excution.log", true);
#endif

static std::map<std::thread::id, std::string> gThreadName;

void SetThreadName(const std::string& name)
{
	gThreadName[std::this_thread::get_id()] = name;
}

template<SeverityType Severity>
class LogHelper
{
public:
	LogHelper(const char* file, int line)
		: mFile(file), mLine(line)
	{
		static_assert(Severity >= SeverityType::debug && Severity <= SeverityType::warning, "Severity type is wrong.");
	}

	~LogHelper()
	{
		// output log
		gLogger.Print<Severity>(getHeader(), mSStream.str());
	}

	template<typename T>
	LogHelper& operator << (const T& arg)
	{
		mSStream << arg;
		return *this;
	}

	template<typename... Args>
	void operator() (Args&&... args)
	{
		(mSStream << ... << args); // must (...)
	}
private:
	std::string getHeader()
	{
		auto now = std::chrono::system_clock::now();
		auto ms = now.time_since_epoch();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ms).count();
		auto localt = std::chrono::system_clock::to_time_t(now);
		std::stringstream time;
		time << std::put_time(std::localtime(&localt), "%Y-%m-%d %H:%M:%S") << ":" << diff % 1000;

		std::stringstream headerStream;
		headerStream << "[" << gSeverityMap[Severity] << " " << time.str() << "]";
		headerStream << "[" << gThreadName[std::this_thread::get_id()] << ":" << std::this_thread::get_id() << "]";
		std::filesystem::path p = mFile;
		std::string_view sv = p.filename().string();
		sv.remove_prefix(1);
		sv.remove_suffix(1);
		headerStream << "[" << sv << ":" << mLine << "]";

		return headerStream.str();
	}

private:
	std::stringstream	mSStream;
	const char*			mFile;
	int					mLine;
};


}


#endif // !_LOG_H_

