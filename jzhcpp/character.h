#ifndef CHARACTER_H_
#define CHARACTER_H_
#include <string>
#include <sstream>
#include <vector>


namespace Character
{
	namespace {
		inline void unpackConcat(std::stringstream&) {}

		template<typename T, typename... Args>
		inline void unpackConcat(std::stringstream& strs, T&& value, Args&&... args)
		{
			strs << std::forward<T>(value);
			unpackConcat(strs, std::forward<Args>(args)...);
		}
	}

	// <字符串连接>
	template<typename... Args>
	inline std::string StrConcat(Args&&... args)
	{
		std::stringstream strs;
		unpackConcat(strs, std::forward<Args>(args)...);
		return strs.str();
	}

	namespace Trim {
		inline bool is_space(char c) {
			//<水平制表符, 换行, 垂直制表符, 回车, 空格>
			return(char(9) == c || char(10) == c || char(11) == c || char(13) == c || char(32) == c);
		}

		template<typename Fun>
		inline void trim_left_if(std::string& input, Fun fun)
		{
			for (auto iter = input.begin(); iter != input.end(); ) {
				if (fun(*iter)) {
					iter = input.erase(iter);
					continue;
				} else {
					break;
				}
			}
		}

		template<typename Fun>
		inline std::string trim_left_copy_if(const std::string& input, Fun fun)
		{
			for (auto iter = input.cbegin(); iter != input.cend(); ) {
				if (fun(*iter)) {
					++iter;
				} else {
					return std::string(iter, input.end());
				}
			}

			return std::string();
		}

		template<typename Fun>
		inline void trim_right_if(std::string& input, Fun fun)
		{
			for (auto iter = input.rbegin(); iter != input.rend(); ) {
				if (fun(*iter)) {
					input.pop_back();
					++iter;
				} else {
					break;
				}
			}
		}

		template<typename Fun>
		inline std::string trim_right_copy_if(const std::string& input, Fun fun)
		{
			for (auto iter = input.crbegin(); iter != input.crend(); ) {
				if (fun(*iter)) {
					++iter;
				} else {
					auto out = std::string(iter, input.crend());
					std::reverse(out.begin(), out.end());
					return out;
				}
			}
			return std::string();
		}

		template<typename Fun>
		inline void trim_if(std::string& input, Fun fun)
		{
			trim_left_if(input, fun);
			trim_right_if(input, fun);
		}

		template<typename Fun>
		inline std::string trim_copy_if(const std::string& input, Fun fun)
		{
			auto t = trim_left_copy_if(input, fun);
			return trim_right_copy_if(t, fun);
		}

		inline std::string trim_copy(const std::string& input)
		{
			return trim_copy_if(trim_left_copy_if(input, is_space), is_space);
		}

	}

	template<typename T>
	inline std::vector<std::string> StrListConvert(const std::vector<T>& list)
	{
		std::stringstream str;
		std::vector<std::string> out;
		for (auto& val : list) {
			str << val;
			out.push_back(str.str());
			str.str("");
		}

		return out;
	}

	inline bool StrIsEmpty(const std::string& str)
	{
		return Trim::trim_copy_if(str, Trim::is_space).empty();
	}

	inline int StrToNum(const std::string& str)
	{
		return std::stoi(str);
	}

	std::vector<std::string> StrSplit(const std::string& str, const std::string& split);

	std::vector<std::string> StrSplitTrim(const std::string& str, const std::string& split);

	std::string StrJoin(const std::vector<std::string>& list, const std::string& join);

	std::string StrToUpper(const std::string& str);

	std::string StrToLower(const std::string& str);

	std::string StrReplace(const std::string& str, const std::string& rep, const std::string& repTo);
}


#endif // !CHARACTER_H_
