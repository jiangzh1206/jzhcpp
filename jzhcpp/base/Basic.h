// Basic.h
// Basic Defined.
// Copyleft 2014 - 2018, jzh just do it

#ifndef BASE_H
#define BASE_H

#if defined _MSC_VER
#include <intrin.h>
#define INCRC(x)	(_InterlockedIncrement((volatile long*)(x)))
#define DECRC(x)	(_InterlockedDecrement((volatile long*)(x)))

#else // gcc

#endif // DEBUG

// <stl>
#include <cstdint>
#include <string>
#include <vector>

namespace yep {

// <type>
typedef int8_t				int8;
typedef int16_t				int16;
typedef int32_t				int32;
typedef int64_t				int64;
typedef uint8_t				uint8;
typedef uint16_t			uint16;
typedef uint32_t			uint32;
typedef uint64_t			uint64;


class NoneCopyable {
public:
	NoneCopyable() = default;
	NoneCopyable(const NoneCopyable&) = delete;
	NoneCopyable& operator=(const NoneCopyable&) = delete;
};

#define NONECOPYABLE	: public yep::NoneCopyable

// <Error base class>
class Error {
private:
	const char* description_;
public:
	Error(const char* description) : description_(description){}

	const char* Description()const { return description_; }
};

}






#endif // !BASE_H