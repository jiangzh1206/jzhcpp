#ifndef YEP_TUPLE_H
#define YEP_TUPLE_H

namespace yep {
	template<typename... Types>
	struct ValueHolder;

	template<typename T>
	struct ValueHolder<T>
	{
		T value_;
		constexpr ValueHolder(T v) : value_(v) {}
		constexpr ValueHolder() = default;
	};

	template<typename T>
	struct ValueHolder<T&>
	{
		T& value_;
		constexpr ValueHolder(T& v) : value_(v) {}
	};

	template<typename T, typename... Types>
	struct ValueHolder<T, Types...>
	{
		T value_;
		ValueHolder<Types...> other_;

		constexpr ValueHolder() = default;
		constexpr ValueHolder(T v, Types... args) : value_(v), other_(args...) {}
	};

	template<typename T, typename... Types>
	struct ValueHolder<T&, Types&...>
	{
		T& value_;
		ValueHolder<Types&...> other_;

		constexpr ValueHolder(T& v, Types&... args) : value_(v), other_(args...) {}
	};

	// <assign valueholder>
	template<typename T1, typename T2>
	constexpr void AssignValueHolder(ValueHolder<T1&>& left, const ValueHolder<T2>& right)
	{
		left.value_ = right.value_;
	}

	template<typename T1, typename T2, typename... Types1, typename... Types2>
	constexpr void AssignValueHolder(ValueHolder<T1&, Types1&...>& left, const ValueHolder<T2, Types2...>& right)
	{
		left.value_ = right.value_;
		AssignValueHolder(left.other_, right.other_);
	}

	// <tuple>
	template<typename... Types>
	struct YepTuple
	{
		ValueHolder<Types...> values_;

		// <else construct>
		template<typename... Types1>
		constexpr YepTuple(Types1&&... args) : values_(std::forward<Types1>(args)...)
		{
		}

		constexpr YepTuple() = default;
	}; 

	template<typename... Types>
	struct YepTuple<Types&...>
	{
		ValueHolder<Types&...> values_;

		constexpr YepTuple(Types&... args) : values_(args...)
		{
		}

		constexpr YepTuple& operator=(const YepTuple<Types...>& other)
		{
			AssignValueHolder(values_, other.values_);
			return *this;
		}

		template<typename... Types2>
		constexpr YepTuple<Types&...>& operator =(YepTuple<Types2...>&& other)
		{
			AssignValueHolder(values_, std::forward<YepTuple<Types2...>>(other).values_);
			return *this;
		}
	};

	// <index iterator>
	template<size_t current, size_t sought, typename... Types>
	struct TupleIndexIterator {};

	template<size_t sought, typename T, typename... Types>
	struct TupleIndexIterator<sought, sought, T, Types...>
	{
		typedef T type;
		static constexpr type& get(ValueHolder<T, Types...>& vh) 
		{
			return vh.value_;
		}
	};
	// < find sought>
	template<size_t current, size_t sought, typename T, typename... Types>
	struct TupleIndexIterator<current, sought, T, Types...>
	{
		typedef typename TupleIndexIterator<current + 1, sought, Types...>::type type;
		static constexpr type& get(ValueHolder<T, Types...>& vh)
		{
			return TupleIndexIterator<current + 1, sought, Types...>::get(vh.other_);
		}
	};

	// <type iterator>
	template<typename... Types>
	struct TupleTypeIterator {};

	// <the T == head>
	template<typename T, typename... Types>
	struct TupleTypeIterator<T, T, Types...> 
	{
		typedef T type;
		static constexpr type& get(ValueHolder<T, Types...>& vh)
		{
			return vh.value_;
		}
	};

	template<typename T, typename Head, typename... Tails>
	struct TupleTypeIterator<T, Head, Tails...>
	{
		// <return Tuple<T, Tails...> head>
		typedef typename TupleTypeIterator<T, Tails...>::type type;
		static constexpr type& get(ValueHolder<Head, Tails...>& vh)
		{
			return TupleTypeIterator<T, Tails...>::get(vh.other_);
		}
	};

	// <get value>
	template<size_t index, typename... Types>
	constexpr typename TupleIndexIterator<0, index, Types...>::type& Get(YepTuple<Types...>& tp) noexcept
	{
		return TupleIndexIterator<0, index, Types...>::get(tp.values_);
	}
	template<typename T, typename... Types>
	constexpr typename TupleTypeIterator<T, Types...>::type& Get(YepTuple<Types...>& tp) noexcept
	{
		return TupleTypeIterator<T, Types...>::get(tp.values_);
	}

	struct Ignore
	{
		template<class T>
		constexpr void operator=(const T& other) const {}
	};

	// <tuple size>
	template<typename... Types>
	struct TypeCounter 
	{
		enum { value = 0 };
	};

	template<typename T, typename... Types>
	struct TypeCounter<T, Types...>
	{
		enum { value = typename TypeCounter<Types...>::value + 1 };
	};

	template<typename... Types>
	struct TupleSize{};

	template<typename... Types>
	struct TupleSize<YepTuple<Types...>>
	{
		enum { value = TypeCounter<Types...>::value };
	};

	// <make tuple>
	template<typename... Types>
	constexpr YepTuple<Types...> MakeTuple(Types&&... args)
	{
		return YepTuple<Types...>(std::forward<Types>(args)...);
	}

	// <tie left refrence>
	template<typename... Types>
	constexpr YepTuple<Types&...> Tie(Types&... args) noexcept
	{
		return MakeTuple(args...);
	}

	// <element type>
	template<size_t index, typename T>
	struct TupleElem {};

	template<size_t index, typename... Types>
	struct TupleElem<index, YepTuple<Types...>>
	{
		typedef typename TupleIndexIterator<0, index, Types...>::type Type;
	};


	constexpr Ignore ignore{};
}

#endif // !YEP_TUPLE_H
