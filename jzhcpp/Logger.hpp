#pragma once

#include <fstream>
#include <string>
#include <mutex>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace LOGZ
{

    typedef unsigned long DWORD;
    extern "C" {
        // win32 获取线程ID
        __declspec(dllimport) DWORD __stdcall GetCurrentThreadId();
    }

    // 策略接口
    class LogPolicyInterface
    {
    public:
        virtual void open_stream() = 0;
        virtual void close_stream() = 0;
        virtual void write(const std::string& msg) = 0;
    };

    class LogFilePolicy : public LogPolicyInterface
    {
        std::unique_ptr<std::ofstream> m_os;
        std::string m_fname;
        unsigned int m_roll_size = 0;
        unsigned int m_write_bytes = 0;
        unsigned int m_file_number = 0;
    public:
        LogFilePolicy(std::string name, int roll_size)
            : m_os(new std::ofstream), m_fname(std::move(name)), m_roll_size(roll_size)
        {
        }

        ~LogFilePolicy()
        {
            if (m_os) {
                close_stream();
            }
        }

        void open_stream() override
        {
            roll_file();
        }
        void close_stream() override
        {
            if (m_os) {
                m_os->close();
            }
        }
        void write(const std::string& msg) override
        {
            auto pos = m_os->tellp();
            (*m_os) << msg << std::endl;
            m_write_bytes += static_cast<unsigned int>(m_os->tellp() - pos);
            if (m_write_bytes > m_roll_size) {
                roll_file();
            }
        }

    private:
        void roll_file()
        {
            if (m_os) {
                m_os->flush();
                m_os->close();
            }

            m_write_bytes = 0;
            m_os.reset(new std::ofstream());

            std::string log_file_name = m_fname;
            log_file_name.append(std::to_string(++m_file_number));

            auto n = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(n);
            std::stringstream str;
            str << std::put_time(std::localtime(&t), "_%Y%m%d_%H%M%S");

            log_file_name.append(str.str());
            log_file_name.append(".Log");
            m_os->open(log_file_name, std::ios_base::binary | std::ofstream::out | std::ofstream::trunc);
            if (!m_os->is_open()) {
                throw(std::runtime_error("Logger: unable to open the out stream!"));
            }
        }
    };

    class LogConsolePolicy : public LogPolicyInterface
    {

    public:
        LogConsolePolicy() {}
        ~LogConsolePolicy() {}

        void open_stream() override {}
        void close_stream() override {}
        void write(const std::string& msg) override
        {
            std::cout << msg << std::endl;
        }
    };

    enum severity_type
    {
        debug = 1,
        error,
        warning
    };

    template <typename LogPolicy>
    class Logger
    {
        std::string get_time()
        {
            time_t raw_time;
            time(&raw_time);
            char ch[64];
            strftime(ch, sizeof(ch), "%Y-%m-%d %H:%M:%S", std::localtime(&raw_time));
            //without the newline character
            return ch;
        }

        std::string get_logline_header()
        {
            std::stringstream header;

            header.str("");
            //header.fill('0');
            //header.width(7);
            header << "[" << get_time();

#ifndef CONSOLELOG
            header << "-";
            header.fill('0');
            header.width(7);
            header << clock();
#endif // !CONSOLELOG
            header << "]";

            return header.str();
        }

        std::stringstream m_log_stream;
        LogPolicy* m_policy;
        std::mutex m_write_mutex;

        void print_impl()
        {
            m_policy->write(get_logline_header() + m_log_stream.str());
            m_log_stream.str("");
        }

        template<typename First, typename...Rest>
        void print_impl(First parm1, Rest...parm)
        {
            m_log_stream << parm1;
            print_impl(parm...);
        }


        Logger(const std::string& name, int roll_size)
        {
            m_policy = new LogPolicy(name, roll_size);
            if (!m_policy) {
                throw std::runtime_error("LOGGER: Unable to create the logger instance");
            }
            m_policy->open_stream();
        }

    public:
        template< severity_type severity, typename...Args >
        void print(const char* func, int line, Args...args)
        {
            m_write_mutex.lock();
            switch (severity) {
            case severity_type::debug:
                m_log_stream << "[DEBUG]" << "[" << GetCurrentThreadId() << "]" << "[" << func << "@" << line << "]";
                break;
            case severity_type::warning:
                m_log_stream << "[WARNING]" << "[" << GetCurrentThreadId() << "]" << "[" << func << "@" << line << "]";
                break;
            case severity_type::error:
                m_log_stream << "[ERROR]" << "[" << GetCurrentThreadId() << "]" << "[" << func << "@" << line << "]";
                break;
            };
            print_impl(args...);
            m_write_mutex.unlock();
        }

        ~Logger()
        {
            if (m_policy) {
                m_policy->close_stream();
                delete m_policy;
            }
        }

        static Logger* instance()
        {
#ifdef CONSOLELOG
            static Logger<LogConsolePolicy> log_inst("console");
#else
            static Logger<LogFilePolicy> log_inst("exec", 1024 * 1024); // 1M
#endif
            return &log_inst;
        }
    };



    template <severity_type type>
    class LoggerAux
    {
    public:
        const char* m_func;
        int m_line;
        std::stringstream m_ss;

        LoggerAux(int line, const char* func)
            : m_line(line), m_func(func)
        {
        }

        ~LoggerAux()
        {
#ifdef CONSOLELOG
            Logger<LogConsolePolicy>::instance()->print<type>(m_func, m_line, m_ss.str());
#else
            Logger<LogFilePolicy>::instance()->print<type>(m_func, m_line, m_ss.str());
#endif
        }

        template<typename T>
        LoggerAux& operator<<(const T& data)
        {
            m_ss << data;
            return *this;
        }
    };


} // LOGZ

#ifdef LOGGING_LEVEL_1

#ifdef CONSOLELOG
#define _LOG(...)       Logger<LogConsolePolicy>::instance()->print<LOGZ::severity_type::debug>(__FUNCTION__, __LINE__, __VA_ARGS__)
#define _LOG_ERR(...)   Logger<LogConsolePolicy>::instance()->print<LOGZ::severity_type::error>(__FUNCTION__, __LINE__, __VA_ARGS__)
#define _LOG_WARN(...)  Logger<LogConsolePolicy>::instance()->print<LOGZ::severity_type::warning>(__FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define _LOG(...)       Logger<LogFilePolicy>::instance()->print<LOGZ::severity_type::debug>(__FUNCTION__, __LINE__, __VA_ARGS__)
#define _LOG_ERR(...)   Logger<LogFilePolicy>::instance()->print<LOGZ::severity_type::error>(__FUNCTION__, __LINE__, __VA_ARGS__)
#define _LOG_WARN(...)  Logger<LogFilePolicy>::instance()->print<LOGZ::severity_type::warning>(__FUNCTION__, __LINE__, __VA_ARGS__)
#endif

#define LOG(...)        _LOG(__VA_ARGS__)
#define LOG_ERR(...)    _LOG_ERR(__VA_ARGS__)
#define LOG_WARN(...)   _LOG_WARN(__VA_ARGS__)

#define _LOGD(line, func) LoggerAux<LOGZ::severity_type::debug>(line, func)
#define _LOGE(line, func) LoggerAux<LOGZ::severity_type::error>(line, func)
#define _LOGW(line, func) LoggerAux<LOGZ::severity_type::warning>(line, func)
#define LOGD _LOGD(__LINE__, __FUNCTION__)
#define LOGE _LOGE(__LINE__, __FUNCTION__)
#define LOGW _LOGW(__LINE__, __FUNCTION__)
#else
#define LOG(...) 
#define LOG_ERR(...)
#define LOG_WARN(...)
#endif

