#pragma once
#include <map>
#include <functional>


namespace MCPP{

/*!
* object fatories
* type表示object（整数/字符串），拿value换取type
* ObjectCreator可以是std::function
*/
template<typename AbstractObject, typename IdentifierType, 
    typename ObjectCreator = std::function<AbstractObject*(void)>>
class Factory
{
public:
    bool Register(const IdentifierType& id, ObjectCreator creator)
    {
        return m_creators.insert(typename AssocMap::value_type(id, creator)).second;
    }

    bool UnRegister(const IdentifierType& id)
    {
        return m_creators.erase(id) == 1;
    }

    AbstractObject* CreateObject(const IdentifierType& id)
    {
        auto iter = m_creators.find(id);
        if (iter != m_creators.end()) {
            return (iter->second)();
        }

        // maybe no register
        assert(false);
        return nullptr;
    }

private:
    using AssocMap = std::map<IdentifierType, ObjectCreator>;
    AssocMap m_creators;
};

} // MCPP