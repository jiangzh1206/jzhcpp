#pragma once
#include <tuple>
#include <type_traits>

//! struct reflection

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

//! 遍历strcut field
template<typename Struct, typename F>
inline constexpr void for_each_field(Struct&& st, F&& func)
{
    constexpr auto struct_tuple = struct_reflect<std::decay_t<Struct>>();
    static_assert(std::tuple_size_v<decltype(struct_tuple)> > 0, "struct filed size should be greater than 0, maybe unset reflection!");

    detail::for_each_tuple(struct_tuple, [&st, &func](auto&& filed_tuple) {
        using Tuple = decltype(filed_tuple);
        static_assert(std::tuple_size_v<std::decay_t<Tuple>> == 2, "filed tuple size should be equal 2");

        //! func(name, field) field成员offset
        func(std::get<0>(std::forward<Tuple>(filed_tuple)), st.*(std::get<1>(std::forward<Tuple>(filed_tuple))));
    });
}


//! tuple<tuple<name0,filed0>,tuple<name1,filed1>...>
//! eg:REFLEC_STRUCT(User1, STRUCT_FIELD("id", id), STRUCT_FIELD("name", name));
#define REFLEC_STRUCT(Struct, ...)                                                                                     \
    template<>                                                                                                         \
    inline constexpr auto struct_reflect<Struct>()                                                                     \
    {                                                                                                                  \
        using _Struct_Type = Struct;                                                                                   \
        return std::make_tuple(__VA_ARGS__);                                                                           \
    }

//! 把每个字段都变成tuple<name, &field>，成员offset
#define STRUCT_FIELD(Name, Field)   std::make_tuple(Name, &_Struct_Type::Field)
#define FIELD(f)     std::make_tuple(#f, &_Struct_Type::f)


//! msvc下得加一层MACRO_COUNT_TEMP！！
#define MACRO_COUNT_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,N, ...) N
#define MACRO_COUNT_TEMP(args)  MACRO_COUNT_IMPL args
#define MACRO_COUNT(...)        MACRO_COUNT_TEMP((__VA_ARGS__,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

//! 连接，展开成f1(args...)
//! cat连接需3层
#define MACRO_GLUE(F, args)         F args
#define MACRO_CONCAT_IMPL2(R, N)    R##N
#define MACRO_CONCAT_IMPL1(R, N)    MACRO_CONCAT_IMPL2(R, N)
#define MACRO_CONCAT(R, N)          MACRO_CONCAT_IMPL1(R, N)



//! REFLECTION(struct, field0, field1)
#define REFLECTION(S, ...)  \
    MACRO_GLUE(MACRO_CONCAT(REFLECT_, MACRO_COUNT(__VA_ARGS__)), (S, __VA_ARGS__))


//! 不同个数参数实现
#define REFLECT_1(S, a1)                 REFLEC_STRUCT(S, FIELD(a1))
#define REFLECT_2(S, a1, a2)             REFLEC_STRUCT(S, FIELD(a1), FIELD(a2))
#define REFLECT_3(S, a1, a2, a3)         REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3))
#define REFLECT_4(S, a1, a2, a3, a4)     REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4))
#define REFLECT_5(S, a1, a2, a3, a4, a5) REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5))
#define REFLECT_6(S, a1, a2, a3, a4, a5, a6)                                                                           \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6))
#define REFLECT_7(S, a1, a2, a3, a4, a5, a6, a7)                                                                       \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7))
#define REFLECT_8(S, a1, a2, a3, a4, a5, a6, a7, a8)                                                                   \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8))
#define REFLECT_9(S, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                                               \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8), FIELD(a9))
#define REFLECT_10(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)                                                         \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10))
#define REFLECT_11(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)                                                    \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11))
#define REFLECT_12(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)                                               \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12))
#define REFLECT_13(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)                                          \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13))
#define REFLECT_14(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)                                     \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13), FIELD(a14))
#define REFLECT_15(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)                                \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13), FIELD(a14), FIELD(a15))
#define REFLECT_16(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16)                           \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13), FIELD(a14), FIELD(a15), FIELD(a16))
#define REFLECT_17(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17)                      \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13), FIELD(a14), FIELD(a15), FIELD(a16),       \
                  FIELD(a17))
#define REFLECT_18(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18)                 \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13), FIELD(a14), FIELD(a15), FIELD(a16),       \
                  FIELD(a17), FIELD(a18))
#define REFLECT_19(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19)            \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13), FIELD(a14), FIELD(a15), FIELD(a16),       \
                  FIELD(a17), FIELD(a18), FIELD(a19))
#define REFLECT_20(S, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20)       \
    REFLEC_STRUCT(S, FIELD(a1), FIELD(a2), FIELD(a3), FIELD(a4), FIELD(a5), FIELD(a6), FIELD(a7), FIELD(a8),           \
                  FIELD(a9), FIELD(a10), FIELD(a11), FIELD(a12), FIELD(a13), FIELD(a14), FIELD(a15), FIELD(a16),       \
                  FIELD(a17), FIELD(a18), FIELD(a19), FIELD(a20))
