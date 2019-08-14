#ifndef VARIANT_H
#define VARIANT_H

#include "Define.h"
#include <time.h>
#include "typecast.h"

template<typename T>
inline T default2()
{
	return 0;
}

template<>
inline std::string default2()
{
	return std::string();
}

//     template<>
//     inline datatime default2()
//     {
//         return datatime;
//     }

template<typename T>
T convert_v(const std::string& val)
{
	if (!val.empty()) {
		try {
			return TypeCast::lexical_cast<T>(val.c_str());
		} catch (...) {}
	}
	return default2<T>(); // can not convert
}

template<typename T>
std::string convert_s(const T& val) 
{
	try {
		return TypeCast::lexical_cast<std::string>(val);
	} catch(...) {}

	return std::string();
}

class Variant{
	enum Type
	{
		vt_null, vt_bool,
		vt_char, vt_uchar,
		vt_short, vt_ushort,
		vt_int, vt_uint,
		vt_long, vt_longlong, vt_ulonglong,
		vt_float, vt_double,
		vt_charstar, vt_string, vt_datetime
	};
public:
	Variant() : m_type(vt_null) {};

	Variant(const Variant& rhs) 
	: m_value(rhs.m_value)
	, m_type(rhs.m_type)
	{
		;
	}

	Variant& operator=(const Variant& rhs)
	{
		if (this != &rhs) {
			m_value = rhs.m_value;
			m_type = rhs.m_type;
		}
		return *this;
	}

public:
	inline bool operator==(const Variant& rhs)
	{
		return m_value == rhs.m_value && m_type == rhs.m_type;
	}

	inline bool operator !=(const Variant& rhs)
	{
		return m_value != rhs.m_value || m_type != rhs.m_type;
	}

public:
	Variant(bool val)
	{
		m_value = convert_s<bool>(val);
		check_set_type(vt_bool);
	}

	Variant(char val)
	{
		m_value = val;
		check_set_type(vt_char);
	}

	Variant(unsigned char val)
	{
		m_value = val;
		check_set_type(vt_uchar);
	}

	Variant(short val)
	{
		m_value = convert_s<short>(val);
		check_set_type(vt_short);
	}

	Variant(unsigned short val)
	{
		m_value = convert_s<unsigned short>(val);
		check_set_type(vt_ushort);
	}

	Variant(int val)
	{
		m_value = convert_s<int>(val);
		check_set_type(vt_int);
	}

	Variant(long val)
	{
		m_value = convert_s<long>(val);
		check_set_type(vt_long);
	}

	Variant(long long val)
	{
		m_value = convert_s<long long>(val);
		check_set_type(vt_longlong);
	}

	Variant(unsigned long long val)
	{
		m_value = convert_s<unsigned long long>(val);
		check_set_type(vt_ulonglong);
	}

	Variant(double val)
	{
		m_value = convert_s<double>(val);
		check_set_type(vt_double);
	}

	Variant(float val)
	{
		m_value = convert_s<float>(val);
		check_set_type(vt_float);
	}

	Variant(const char* val)
	{
		m_value = (val ? std::string(val) : std::string());
		check_set_type(vt_charstar);
	}

	Variant(std::string val)
	{
		m_value = val;
		check_set_type(vt_string);
	}

	bool tobool()const
	{
		return convert_v<bool>(m_value);
	}

	short toshort()const
	{
		return convert_v<short>(m_value);
	}

	unsigned short toushort()const
	{
		return convert_v<unsigned short>(m_value);
	}

	int toint()const
	{
		return convert_v<int>(m_value);
	}

	unsigned int touint()const
	{
		return convert_v<unsigned int>(m_value);
	}

	long tolong()const
	{
		return convert_v<long>(m_value);
	}

	long long tolonglong()const
	{
		return convert_v<long long>(m_value);
	}

	unsigned long long toulonglong()const
	{
		return convert_v<unsigned long long>(m_value);
	}

	double todouble()const
	{
		return convert_v<double>(m_value);
	}

	float tofloat()const
	{
		return convert_v<float>(m_value);
	}

	std::string tostring()const
	{
		return m_value;
	}
public:
	inline operator bool()
	{
		return tobool();
	}

	inline operator short()
	{
		return toshort();
	}

	inline operator unsigned short()
	{
		return toushort();
	}

	inline operator int()
	{
		return toint();
	}

	inline operator unsigned int()
	{
		return touint();
	}

	inline operator long long()
	{
		return tolonglong();
	}

	inline operator unsigned long long()
	{
		return toulonglong();
	}

	inline operator float()
	{
		return tofloat();
	}

	inline operator double()
	{
		return todouble();
	}

	inline operator std::string()
	{
		return tostring();
	}

	inline Type GetType() const 
	{ 
		return m_type;
	}

private:
	void check_set_type(Type type)
	{
		m_type = !m_value.empty() ? type : vt_null;
	}
private:
	std::string m_value;
	Type m_type;
};

typedef std::vector<Variant> VarList;


#endif // !VARIANT_H