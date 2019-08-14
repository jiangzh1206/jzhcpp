#pragma once

/*
*   模板偏特化
*   局部类
*   型别和数值映射(Int2Type,Type2Int class templates)
*   可转换
*   NullType EmptyType
*   Traits
*/

// 0 array 非法
#define STATIC_CHECK(expr) { char unnamed[(expr) : 1 ? 0]; }

namespace MCPP
{
// 1.局部类, 某个接口转换成另个接口
class Interface
{
public:
    virtual void Fun() = 0;
};

template<typename T, typename U>
Interface* MakeAdapter(const T& obj, const U& arg)
{
    class Local : public Interface
    {
        T obj_;
        U arg_;
    public:
        Local(const T& obj, const U& arg) : obj_(obj), arg_(arg) { }
        void Fun() { obj.Call(arg); }
    };

    return new Local(obj, arg);
    std::cout << "local class" << "\n";
}

// 2.整数映射到类型 编译期分派 Int2Type<0> Int2Type<1>不同类型
template<int V>
struct Int2Type
{
    enum { value = V };
};

// 类型映射到类型
template<typename T>
struct Type2Type
{
    using OriginalType = T;
};

template<typename T, bool IS>
class NiftyContainer
{
    void DoSomething(T* obj, Int2Type<true>)
    {
        // obj do something
        std::cout << "do something true" << "\n";
    }

    // 重载
    void DoSomething(T* obj, Int2Type<false>)
    {
        // obj do something
        std::cout << "do something false" << "\n";
    }

public:
    void DoSomething(T* obj)
    {
        DoSomething(obj, Int2Type<IS>());
    }
};

class TObject;
template<typename T, typename U>
T* Create(const U& arg, Type2Type<T>)
{
    return new T(arg);
}

// 3.特化函数模板
template<typename U>
TObject* Create(const U& arg, Type2Type<TObject>)
{
    return new TObject(arg);
}


// 4.From是否可转换成To
template<typename From, typename To>
struct Conversion
{
    using Small = char;
    struct Big 
    { 
        char dummy[2]; 
    };
    
    // 不用实现, sizeof 不对其引数求值
    static Small Test(To);
    static Big Test(...);
    static From MakeT();
    
    // MakeT返回From是否能调用Test(To)
    enum{value = sizeof(Test(MakeT())) == sizeof(Small) };
    // 双向转换
    enum{con2way = value && Conversion<To, From>::value};
    // 相同类型
    enum { same = 0 };
};

// 相同类型特化
template<typename T>
struct Conversion<T, T>
{
    enum { value = 1};
    enum { con2way = 1 };
    enum { same = 1 };
};

// 5.NullType 不能生成对象, EmptyType 可被继承,可视为template缺省
class NullType; 
class EmptyType{};

}
