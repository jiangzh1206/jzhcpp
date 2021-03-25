#pragma once
#include<iostream>
namespace tymeta
{
	using std::string;

	template<class T1, class ...Args>
	struct type_descriptor {
	};

	template<class T>
	std::ostream& operator<<(std::ostream& os, type_descriptor<T>) {
		os << "[unknown type, maybe \"" << typeid(T).name() << "\"]";
		return os;
	}

	template<class r>
	std::ostream& operator<<(std::ostream& os, type_descriptor<r()>) {
		os << "function () -> " << type_descriptor<r>();
		return os;
	}

	template<class r, class ...Args>
	std::ostream& operator<<(std::ostream& os, type_descriptor<r(Args...)>) {
		os << "function ( " << type_descriptor<Args...>() << " ) -> " << type_descriptor<r>();
		return os;
	}

	template<class first_arg, class ...Args>
	std::ostream& operator<<(std::ostream& os, type_descriptor<first_arg, Args...>) {
		os << type_descriptor<first_arg>() << ", " << type_descriptor<Args...>();
		return os;
	}

	template<class R>
	std::ostream& operator<<(std::ostream& os, type_descriptor<R&>) {
		os << "reference to " << type_descriptor<R>();
		return os;
	}

	template<class R>
	std::ostream& operator<<(std::ostream& os, type_descriptor<const R&>) {
		os << "const reference to " << type_descriptor<R>();
		return os;
	}

	template<class p>
	std::ostream& operator<<(std::ostream& os, type_descriptor<p*>) {
		os << "pointer to " << type_descriptor<p>();
		return os;
	}

	template<class p>
	std::ostream& operator<<(std::ostream& os, type_descriptor<const p*>) {
		os << "pointer to const " << type_descriptor<p>();
		return os;
	}

	template<class p>
	std::ostream& operator<<(std::ostream& os, type_descriptor<volatile p*>) {
		os << "pointer to volatile " << type_descriptor<p>();
		return os;
	}

	template<class T>
	std::ostream& operator<<(std::ostream& os, type_descriptor<volatile T>) {
		os << "volatile " << type_descriptor<T>();
		return os;
	}

	template<class T>
	std::ostream& operator<<(std::ostream& os, type_descriptor<const T>) {
		os << "const " << type_descriptor<T>();
		return os;
	}

	template<class a>
	std::ostream& operator<<(std::ostream& os, type_descriptor<a[]>) {
		os << "array of " << type_descriptor<a>();
		return os;
	}

	template<class a, size_t N>
	std::ostream& operator<<(std::ostream& os, type_descriptor<a[N]>) {
		os << "array (size " << N << ") of " << type_descriptor<a>();
		return os;
	}

	template<class T = int>
	std::ostream& operator<<(std::ostream& os, type_descriptor<int>) {
		os << "int";
		return os;
	}

	template<class T = float>
	std::ostream& operator<<(std::ostream& os, type_descriptor<float>) {
		os << "float";
		return os;
	}

	template<class T = double>
	std::ostream& operator<<(std::ostream& os, type_descriptor<double>) {
		os << "double";
		return os;
	}

	template<class T = long double>
	std::ostream& operator<<(std::ostream& os, type_descriptor<long double>) {
		os << "long double";
		return os;
	}

	template<class T = bool>
	std::ostream& operator<<(std::ostream& os, type_descriptor<bool>) {
		os << "bool";
		return os;
	}

	template<class T = char>
	std::ostream& operator<<(std::ostream& os, type_descriptor<char>) {
		os << "char";
		return os;
	}

	template<class T = wchar_t>
	std::ostream& operator<<(std::ostream& os, type_descriptor<wchar_t>) {
		os << "wchar_t";
		return os;
	}

	template<class T = short>
	std::ostream& operator<<(std::ostream& os, type_descriptor<short>) {
		os << "short";
		return os;
	}

	template<class T = long>
	std::ostream& operator<<(std::ostream& os, type_descriptor<long>) {
		os << "long";
		return os;
	}

	template<class T = void>
	std::ostream& operator<<(std::ostream& os, type_descriptor<void>) {
		os << "void";
		return os;
	}

}