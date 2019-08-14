#ifndef DATABASE_H
#define DATABASE_H

#include "Basic.h"

namespace yep {
namespace database {

// <用户使用SQLModel的特化e.g.struct SqlModel<DBUsers>>
template<typename T>
struct SQLModel
{
	template<typename U>
	static void Serialize(U& a, T& name){}
	static std::string Table() {}
	static std::string PriKey() {}
	static std::string Fields() {}
};

// <外不使用返回static>
template<typename T>
struct SQLTable
{
	static std::string Table()
	{
		static std::string str = SQLModel<T>::Table();
		return str;
	}

	static std::string PriKey()
	{
		static std::string str = SQLModel<T>::PriKey();
		return str;
	}

	static std::string Fields()
	{
		static std::string str = SQLModel<T>::Fields();
		return str;
	}

	// TODO: character
#if 0
	static std::string Values()
	{
		static std::string str = StrConcat(SQLModel<T>::PriKey(), ",", SQLModel<T>::Fields());
		return str;
	}

	static std::vector<std::string> PriKeyList()
	{
		static std::vector<std::string> list(Character::StrSplitTrim(SQLModel<T>::PriKey(), ","));
		return list;
	}

	static std::vector<std::string> FieldList()
	{
		static std::vector<std::string> list(Character::StrSplitTrim(SQLModel<T>::Fields(), ","));
		return list;
	}

	static std::vector<std::string> ValueList()
	{
		static std::vector<std::string> list(Character::StrSplitTrim(Values(), ","));
		return list;
	}
#endif


};


}

} // namespace yep

#endif // !DATABASE_H

