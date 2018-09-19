#ifndef _LOGGER_H
#define _LOGGER_H

#include "log.hpp"

#define LOG_DEUBG Logging::LogHelper<Logging::SeverityType::debug>(__FILE__, __LINE__)
#define LOG_ERROR Logging::LogHelper<Logging::SeverityType::error>(__FILE__, __LINE__)
#define LOG_WARN Logging::LogHelper<Logging::SeverityType::warning>(__FILE__, __LINE__)
#define LOG_SET_THREAD_NAME(name) Logging::SetThreadName(name)


#endif // !_LOGGER_H

