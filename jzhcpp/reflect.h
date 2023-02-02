#pragma once
#include <tuple>
#include <type_traits>

namespace detail
{

template<typename F, typename Tuple, std::size_t... I>
inline constexpr void for_each_tuple(Tuple&& tuple, F&& fn, std::index_sequence<I...>)
{
    //! 遍历每个tuple<name0,filed0>
    //! 逗号表达式解包 arr[]={0,0,0...}
    int arr[] = {(fn(std::get<I>(std::forward<Tuple>(tuple))), 0)...};
}

template<typename F, typename Tuple>
inline constexpr void for_each_tuple(Tuple&& tuple, F&& fn)
{
    for_each_tuple(std::forward<Tuple>(tuple), std::forward<F>(fn),
                   std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
}

} // namespace detail

template<typename T>
inline constexpr auto struct_reflect()
{
    return std::make_tuple();
}

//! REFLECTION(struct, field0, field1)
//! tuple<tuple<name0,filed0>,tuple<name1,filed1>...>
#define REFLECTION(Struct, ...)                                                                                        \
    template<>                                                                                                         \
    inline constexpr auto struct_reflect<Struct>()                                                                     \
    {                                                                                                                  \
        using Struct_Type = Struct;                                                                                    \
        return std::make_tuple(__VA_ARGS__);                                                                           \
    }

//! 把每个字段都变成tuple<name, &field>，成员offset
#define STRUCT_FIELD(Name, Field) std::make_tuple(Name, &Struct_Type::Field)

//! 遍历strcut field
template<typename Struct, typename F>
inline constexpr void for_each_field(Struct&& st, F&& func)
{
    constexpr auto struct_tuple = struct_reflect<std::decay_t<Struct>>();
    static_assert(std::tuple_size_v<decltype(struct_tuple)> > 0, "struct filed size should be greater than 0");

    detail::for_each_tuple(struct_tuple, [&st, &func](auto&& filed_tuple) {
        using Tuple = decltype(filed_tuple);
        static_assert(std::tuple_size_v<std::decay_t<Tuple>> == 2, "filed tuple size should be equal 2");

        //! func(name, field) field成员offset
        func(std::get<0>(std::forward<Tuple>(filed_tuple)), st.*(std::get<1>(std::forward<Tuple>(filed_tuple))));
    });
}
