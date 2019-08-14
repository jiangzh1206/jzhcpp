#ifndef REFLECT_H
#define REFLECT_H
#include <map>
#include <string>

namespace yep {
namespace reflect {

struct DefaultTag
{
};

template<typename T, typename TAG = DefaultTag>
class BaseFactory {
public:
	virtual T* create() const = 0;
};

template<typename Base, typename Der, typename TAG = DefaultTag>
class Factory : public BaseFactory<Base> {
	virtual Der* create() const {
		return new Der();
	}
};

template<typename Base, typename TAG = DefaultTag>
std::map<std::string, BaseFactory<Base, TAG>*>& GlobalFactoryMap()
{
	static std::map<std::string, BaseFactory<Base>*> globalMap;
	return globalMap;
}

template<typename Base, typename Der, typename Name, typename TAG = DefaultTag>
class RegistClass
{
public:
	RegistClass() {
		GlobalFactoryMap<Base, TAG>().insert(
			std::make_pair<std::string, Factory<Base, Der, TAG>*>(
				std::string(Name::get()), new Factory < Base, Der, TAG>()
			)
		);
	}

	void* get() { return nullptr;}
};

// for inisialize registclass once;
template<typename Base, typename Der, typename Name, typename TAG = DefaultTag>
class RegistClassCaller
{
	static RegistClass<Base, Der, Name, TAG> register_;
public:
	RegistClassCaller() {
		register_.get();
	}

};
// define register_;
template<typename Base, typename Der, typename Name, typename TAG = DefaultTag>
RegistClass<Base, Der, Name, TAG>
RegistClassCaller<Base, Der, Name, TAG>::register_;

#define STRING_WRAPPER(s)		\
struct string_wrapper_##s{		\
	static const char* get(){	\
		return #s;				\
	}							\
};								\

/*! Macro to call when RE_class has only two arguments */
#define RE_class_2(classname, Base)				\
STRING_WRAPPER(classname)						\
static yep::reflect::RegistClassCaller<			\
Base, class classname, string_wrapper_##classname> \
registor_caller_##classname; \
class classname: public Base

/*! Macro to call when RE_class has three arguments */
#define RE_class_3(classname, Base, tag)		\
STRING_WRAPPER(classname)						\
static yep::reflect::RegistClassCaller<			\
Base, class classname, string_wrapper_##classname, tag> \
registor_caller_##classname; \
class classname: public Base

/*! Small thick to overload macro RE_class */
#define __RE_CLASS_GET_MACRO(_1, _2, _3, NAME, ...) NAME

/*! RE_class(Class, BaseClass [, tag]) */
#define RE_class(...) __RE_CLASS_GET_MACRO( \
  __VA_ARGS__, RE_class_3, RE_class_2)(__VA_ARGS__)

/*! Macro to call when Reflect has only one argument */
#define Reflect_1(T) (yep::reflect::GlobalFactoryMap<T>())

/*! Macro to call when Reflect has two arguments */
#define Reflect_2(T, tag) (yep::reflect::GlobalFactoryMap<T, tag>())

#define __REFLECT_GET_MACRO(_1, _2, NAME, ...) NAME

/*! Reflect(BaseClass [, tag]) */
#define Reflect(...) __REFLECT_GET_MACRO( \
  __VA_ARGS__, Reflect_2, Reflect_1)(__VA_ARGS__)


} // namespace yep
} // namespace reflect

#endif // !REFLECT_H
