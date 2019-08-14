#pragma once
#include <iostream>
#include <string>

template <class T>
struct st_sfinae
{
	typedef int iterator;
	int data = 0;
};

template <class T>
inline void f_test_sfinae(const T& t, typename T::iterator* it = nullptr)
{
	std::cout << "normal sfinae" << std::endl;
}

inline void f_test_sfinae(...) 
{
	std::cout << "SFINAE!!!" << std::endl;
}

namespace SFINAE_98 {

template <class T> 
struct hasSerialize
{
	// compile time check size
	typedef char yes;
	typedef yes no[2];

	template <typename U, U u> struct reallyHas;

	template <typename C> 
	static yes& test( reallyHas<std::string(C::*)(), &C::serialize>*) { }

	template <typename C>
	static yes& test(reallyHas<std::string(C::*)()const, &C::serialize>*) { std::cout << "suitation no!" << std::endl; }
	
	template <typename> static no& test(...) {}

	//static const bool value = sizeof(test<T>(0)) == sizeof(yes);

	enum{ value = sizeof(test<T>(0)) == sizeof(yes) };
};

template <bool B, class T = void>
struct enable_if{};

template <class T>
struct enable_if<true, T> {
	typedef T type;
};

template <class T>
typename enable_if<hasSerialize<T>::value, std::string>::type serialize(const T& obj) {
	return obj.serialize();
}

template <class T>
typename enable_if<!hasSerialize<T>::value, std::string>::type serialize(const T& obj) {
	return to_string(obj);
}

} // !SFINAE_98

namespace SFINAE_11{

template <typename T, typename = std::string>
struct hasSerialize : public std::false_type
{
};

template <typename T>
struct hasSerialize<T, decltype(std::declval(T)().serialize())>
	: std::true_type
{
};

template <class T>
typename std::enable_if<hasSerialize<T>::value, std::string>::type serialize(const T& obj) {
	return obj.serialize();
}

template <class T>
typename std::enable_if<!hasSerialize<T>::value, std::string>::type serialize(const T& obj) {
	return to_string(obj);
}

}

#if 0 // ����ʧ��is_validʧ��?

namespace SFINAE_14 {

template <typename UnnamedType> struct container
{
private:
	// remember unamed type;
	template <typename Param> constexpr auto testValidity(int /*unused*/) 
		->decltype(std::declval<UnnamedType>()(std::declval(Param)()), std::true_type()) // ���������, ������ʽ�����,���һ��Ϊ����
	{
		return std::true_type();
	}

	template <typename Param> constexpr std::false_type testValidity(...) {
		return std::false_type();
	}

public:
	template <typename Param> constexpr auto operator()(const Param& /*p*/) {
		// The argument is forwarded to one of the two overloads.
		// The SFINAE on the 'true_type' will come into play to dispatch.
		return testValidity<Param>(int());
	}
};

template <typename UnnamedType>
inline constexpr auto is_valid(const UnnamedType& t) {
	return container<UnnamedType>();
}

static auto hasSerialize = is_valid([](auto&& x) -> decltype(x.serialize()) {});

template <class T> 
inline auto serialize(const T& obj)
-> typename std::enable_if< decltype(hasSerialize(obj))::value, std::string >::type
{
	return obj.serialize();
}

template <class T> 
inline auto serialize(T& obj)
-> typename std::enable_if< !decltype(hasSerialize(obj))::value, std::string >::type
{
	return to_string(obj);
}

}
#endif

    template <typename T, typename = void>
    struct is_smart_pointer : std::false_type
    {
    };
    
    template <typename T>
    struct is_smart_pointer<T,
    std::void_t<decltype(std::declval<T>().operator ->()),
        decltype(std::declval<T>().get())>> : std::true_type
    {
    };