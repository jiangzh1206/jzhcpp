#pragma once
#include <typeinfo>
#include <cassert>

namespace MCPP
{
// std::type_info wrapper
class TypeInfo
{
public:
    TypeInfo();
    TypeInfo(const std::type_info& info);
    const std::type_info& Get() const;
    bool Before(const TypeInfo& rhs) const;
    const char* Name() const;

private:
    const std::type_info* m_info;
};


TypeInfo::TypeInfo()
{
    class Nil {};
    m_info = &typeid(Nil);
    assert(&m_info);
}

TypeInfo::TypeInfo(const std::type_info & info)
{
    m_info = &info;
    assert(&m_info);
}

const std::type_info& TypeInfo::Get() const
{
    assert(m_info);
    return *m_info;
}

bool TypeInfo::Before(const TypeInfo & rhs) const
{
    assert(m_info);
    return m_info->before(*rhs.m_info);
}

const char* TypeInfo::Name() const
{
    assert(m_info);
    return m_info->name();
}

// comparison operators
inline bool operator==(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return lhs.Get() == rhs.Get();
}

inline bool operator<(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return lhs.Before(rhs);
}

inline bool operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs == rhs);
}

inline bool operator>(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return rhs < lhs;
}

inline bool operator<=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs > rhs);
}

inline bool operator>=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs < rhs);
}

}