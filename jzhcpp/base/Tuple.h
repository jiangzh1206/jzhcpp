#ifndef BASE_TUPLE_H
#define BASE_TUPLE_H

namespace yep {

// <declaration>
template<typename... TypeList>
class Tuple;

// <empty>
template<>
class Tuple<>{};
using EmptyTuple = Tuple<>;

// <one element>
template<typename Head>
class Tuple<Head> : EmptyTuple 
{
	Head head_;
public:
	Tuple(const Head& v) : head_(v) {}

	void SetValue(const Head& v) { head_ = v; }

	Head GetHead() { return head_; }
};

// <µÝ¹é¶¨Òå>
template<typename Head, typename... TypeList>
class Tuple<Head, TypeList...> : public Tuple<TypeList...> 
{
	Head head_;
public:

	Tuple(const Head& v, TypeList... list)
		: Tuple<TypeList...>(list...)
		, head_(v) 
	{}

	void SetValue(const Head& v) 
	{ 
		head_ = v; 
	}

	Head GetHead() { return head_; }
};

// < tuple size>
template<typename... TypeList>
struct TypesCounter
{
	enum { value = 0 };
};

template<typename Head, typename... TypeList>
struct TypesCounter<Head, TypeList...>
{
	enum { value = TypesCounter<TypeList...>::value + 1 };
};

template<typename... TypeList>
struct tuple_size {};

template<typename... TypeList>
struct tuple_size<Tuple<TypeList...>>
{
	enum { value = TypesCounter<TypeList...>::value };
};



// <make_tuple>
template<typename... TypeList>
Tuple<TypeList...> MakeTuple(TypeList...)
{
}

template<>
Tuple<> MakeTuple() { return EmptyTuple(); }

template<typename Head, typename... TypeList>
Tuple<Head, TypeList...> MakeTuple(const Head& v, TypeList... list)
{
	return Tuple<Head, TypeList...>(v, list...);
}

// <TupleAt>
template<size_t N, typename... TypeList>
struct TupleAt;

template<size_t N, typename Head, typename... TypeList>
struct TupleAt<N, Tuple<Head, TypeList...>> 
{
	using ValueType = typename TupleAt <N - 1, Tuple<TypeList...>>::ValueType;
	using TupleType = typename TupleAt <N - 1, Tuple<TypeList...>>::TupleType;
};

// <get head>
template<typename Head, typename...TypeList>
struct TupleAt<0, Tuple<Head, TypeList...>>
{
	using ValueType = Head;
	using TupleType = Tuple<Head, TypeList...>;
};

// <empty>
template<size_t N>
struct TupleAt<N, EmptyTuple>
{
	using ValueType = EmptyTuple;
	using TupleType = EmptyTuple;
};

// <get tuple value>
template<size_t N, typename... TypeList>
typename TupleAt<N, Tuple<TypeList...>>::ValueType TupleGet(const Tuple<TypeList...>& t)
{
	return static_cast<typename TupleAt<N, Tuple<TypeList...>>::TupleType>(t).GetHead();
}

// <set value>
template<size_t N, typename... TypeList>
 void TupleSet(Tuple<TypeList...>& t, const typename TupleAt<N, Tuple<TypeList...>>::ValueType& v)
{
	static_cast<typename TupleAt<N, Tuple<TypeList...>>::TupleType&>(t).SetValue(v);
	auto vv = static_cast<typename TupleAt<N, Tuple<TypeList...>>::TupleType>(t).GetHead();
}


}

#endif // !BASE_TUPLE_H

