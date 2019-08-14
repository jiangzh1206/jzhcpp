#pragma once
#include <map>
#include <any>
#include <variant>

class value_t
{
public:
    enum class type_t
    {
        null,
        string,
        boolean,
        number_integer,
        number_unsigned,
        number_floating,
    };

    value_t();
    value_t(const std::string&);
    value_t(bool);
    value_t(int32_t);
    value_t(uint32_t);
    value_t(float);

    value_t(const value_t&);
    value_t& operator=(const value_t&);

    bool operator==(const value_t&);
    bool operator!=(const value_t&);

    operator bool() const {}
    operator std::string() const {}
    operator int32_t() const {}
    operator uint32_t() const {}
    operator float() const {}

private:
    type_t m_type = type_t::null;
    std::variant<bool, int32_t, uint32_t, float, std::string> m_value;
};

class Config : public std::map<std::string, std::any>
{
public:
    static Config& GetCfg();
    bool LoadConfig(const std::string&);

private:
    Config() {};
};
