#pragma once
#include <cstdlib>
#include <iostream>

namespace MCPP {

// policy class
template<typename T>
struct NewCreator 
{
    static T* Create() { return new T; }

protected:
    // 不能delete policy对象(免去写virtual析构)
    ~NewCreator() {}
};

template<typename T>
struct MallocCreateor 
{
    static T* Create() 
    {
        void* p = std::malloc(sizeof(T));
        if (!p)
            return nullptr;
        return new(p) T;
    }

};

template<typename T>
struct PrototypeCreator 
{
    PrototypeCreator(T* obj) : m_prototype(obj) {}

    T* Create() 
    {
        return m_prototype ? m_prototype->Clone() : nullptr;
    }

    T* GetProtoType() { return m_prototype; }
    void SetProtoType(T* obj) { m_prototype = obj; }
private:
    T* m_prototype;
};

// 各个policy class 应该是正交分解(无交互作用)
// host class 
// host class 继承 policy class, 避免delete host class向上转型的policy class
template<typename T, template<typename> class Policy = NewCreator>
class ObjectManager : public Policy<T> 
{
public:
    T* Create()
    {
        return Policy<T>::Create();
    }
};


}; // MCPP