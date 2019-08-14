// Function.h
// Definition of the Function.
// Copyleft- 2018, jzh just do it

#ifndef THREAD_FUNCTION_H
#define THREAD_TIMESPAN_H

#include <functional>

namespace yep{
namespace thread{

template<typename... Args>
class Function
{
public:

	virtual ~Function()
	{
	}

	Function(const std::function<void(Args...)>& fct)
	{
		calls_ = fct;
	}

	Function(std::function<void(Args...)>&& fct)
	{
		calls_ = fct;
	}

	template<typename Object>
	Function(Object* object, void (Object::*method)(Args...))
	{
		printf("connect:: 1\n");
		calls_ = [object, method](Args... args){(object->*method)(args...); };
	}

	template<typename Object>
	Function(Object* object, void (Object::*method)(Args...) const)
	{
		calls_ = [object, method](Args... args){(*object.*method)(args...); };
	}

	template<typename Object>
	Function(const Object* object, void (Object::*method)(Args...) const)
	{
		calls_ = [object, method](Args... args){(*object.*method)(args...); };
	}

	void call(Args... args)
	{
		calls_(args...);
	}


private:
	std::function<void(Args...)> calls_;
};


} // !namespace thread
} // !namespace yep

#endif // !THREAD_FUNCTION_H
