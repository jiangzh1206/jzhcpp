#ifndef YEP_FACTORY_H
#define YEP_FACTORY_H

#include <string>
#include <map>
#include <functional>
#include <type_traits>

namespace yep {

struct Message
{
	virtual void foo(){}
	virtual ~Message(){}

};

// <通过模板参数把构造参数传进来>
template<typename... U>
struct Factory
{
	template<typename T>
	struct register_t
	{
		register_t(int32_t key) {
			Factory<U...>::get().map_.emplace(key, [](U&&...u){return new T(std::forward<U>(u)...);});
		}
		register_t(int32_t key, std::function<Message*(U&&...u)>&& f) {
			Factory<U...>::get().map_.emplace(key, std::move(f));
		}
	};

	inline Message* produce(int32_t key, U&&...u) {
		if (map_.find(key) == map_.end()) {
			throw std::string("the key is invalide.");
		}
		return map_[key](std::forward<U>(u)...);
	}

	inline static Factory<U...>& get() {
		static Factory<U...> instance;
		return instance;
	}

private:
	Factory<U...>() {}
	Factory<U...>(const Factory<U...>&) = delete;
	Factory<U...>(Factory<U...>&&) = delete;

	std::map<int32_t, std::function<Message*(U&&...)>> map_;
};

#define REGISTER_MESSAGE(T, key) static Factory<>::register_t<T> reg_##T##_(key);
// <fun, key, agrs>
#define REGISTER_MESSAGE1(T, key, ...) static Factory<__VA_ARGS__>::register_t<T> reg_##T##_(key);
// <fun, key, labmda, agrs>
#define REGISTER_MESSAGE2(T, key, l, ...) static Factory<__VA_ARGS__>::register_t<T> reg_##T##_(key, l);

template<typename... Args>
inline Message* produce_msg(int32_t key, Args... args) {
	return Factory<Args...>::get().produce(key, std::forward<Args>(args)...);
}

}

#endif // !YEP_FACTORY_H

