#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include "Basic.h"
#include <sstream>

using namespace yep;

namespace serialization {

	template<typename T, typename = void>
	struct is_smart_pointer : std::false_type
	{
	};
	template<typename T>
	struct is_smart_pointer<T, std::void_t<decltype(std::declval<T>().operator ->()),
		decltype(std::declval<T>().get())>> : std::true_type
	{
	};


	template <typename T>
	bool test_trivial(const T& t)
	{
		return std::is_trivial<T>::value;
	}

	template<typename T>
	std::string serialize(const T& t)
	{
		return T::serialize(t);
	}

	template<typename T>
	size_t deserialize(const std::string& v, T& t)
	{
		return T::deserialize(v, t);
	}

	// <size_t>
	template<>
	std::string serialize(const size_t& t)
	{
		std::string v;
		v.append((const char*)&t, sizeof(int));
		return v;
	}
	template<>
	size_t deserialize(const std::string& v, size_t& t)
	{
		memcpy(&t, v.data(), sizeof(t));
		return sizeof(t);
	}

	// <string>
	template<>
	std::string serialize(const std::string& v)
	{
		if (!v.size()) {
			return std::string();
		}
		std::string str;
		str.append(serialize(v.size()));
		str.append(v);
		return str;
	}
	template<>
	size_t deserialize(const std::string& v, std::string& t)
	{
		size_t len = 0;
		deserialize(v, len);
		t = v.substr(sizeof(len), len);
		return sizeof(int) + len;
	}

	// <Marco definition>
#define NORMAL_DATA_SERIALIZE(Type) template<> \
	std::string serialize(const Type& a) \
	{ \
	std::string ans; \
	ans.append((const char*)&a, sizeof(a)); \
	return ans; \
	}
#define NORMAL_DATA_DESERIALIZE(Type) template<> \
	size_t deserialize(const std::string& str, Type& a)\
	{ \
	memcpy(&a, str.data(), sizeof(a)); \
	return sizeof(a); \
	}

	NORMAL_DATA_SERIALIZE(int);
	NORMAL_DATA_DESERIALIZE(int);
	NORMAL_DATA_SERIALIZE(double);
	NORMAL_DATA_DESERIALIZE(double);
	NORMAL_DATA_SERIALIZE(float);
	NORMAL_DATA_DESERIALIZE(float);
	NORMAL_DATA_SERIALIZE(char);
	NORMAL_DATA_DESERIALIZE(char);


	// <Serializable base class>
	template<typename Type>
	class Serializable {
	public:
		static std::string serialize(const Type&);
		static Type deserialize(const std::string&);
	};

	class SerialOutput {
		std::ostringstream os_;
	public:
		SerialOutput() : os_(std::ios::binary) {}
	
		template<typename Type>
		SerialOutput& operator<< (const Type& a)
		{
			std::string v = serialize(a);
			os_.write(v.data(), v.size());
			return *this;
		}

		std::string str() const { return os_.str(); }

		void reset() { os_.str(""); }
	};

	class SerialInput
	{
		std::string input_;
		size_t size_;
	public:
		SerialInput(const std::string& s) : input_(s), size_(s.size()) {}

		template<typename Type>
		SerialInput& operator >>(Type& a)
		{
			auto size = deserialize(input_, a);
			input_ = input_.substr(size);
			return *this;
		}

		void str(const std::string& str) 
		{ 
			input_ = str; 
			size_ = input_.size(); 
		}

		size_t donesize() const 
		{
			return (size_ - input_.size());
		}
	};

}

#endif // !SERIALIZATION_H

