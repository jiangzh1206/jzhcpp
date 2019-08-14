#include "character.h"
#include <algorithm>
#include <cctype>

namespace Character {

std::vector<std::string> StrSplit(const std::string& str, const std::string& split)
{
	std::vector<std::string> strList;

	size_t last = 0;
	auto index = str.find_first_of(split, last);
	while (index != std::string::npos)
	{
		strList.push_back(str.substr(last, index - last));
		last = index + 1;
		index = str.find_first_of(split, last);
	}
	if (str.size() - last > 0)
	{
		strList.push_back(str.substr(last, str.size() - last));
	}
	return strList;
}

std::vector<std::string> StrSplitTrim(const std::string& str, const std::string& split)
{
	std::vector<std::string> strList;

	auto list = StrSplit(str, split);
	for (auto it = list.begin(); it != list.end(); ) {
		Trim::trim_if(*it, Trim::is_space);
		if ((*it).empty()) {
			it = list.erase(it);
		} else {
			++it;
		}
	}
	
	return strList;
}

std::string StrJoin(const std::vector<std::string>& list, const std::string& join)
{
	std::string out;
	for (auto& val : list) {
		out += val + join;
	}
	if (!out.empty()) {
		out.pop_back();
	}
	return out;
}

std::string StrToUpper(const std::string & str)
{
	auto out = str;
	std::transform(out.begin(), out.end(), out.begin(),[](unsigned char c) {
		 return std::toupper(c); }
	);
	return out;
}

std::string StrToLower(const std::string & str)
{
	auto out = str;
	std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
		return std::tolower(c); }
	);
	return out;
}

std::string StrReplace(const std::string& str, const std::string& rep, const std::string& repTo)
{
	std::string out = str;
	size_t last = 0;
	auto index = out.find_first_of(rep, last);
	while (index != std::string::npos)
	{
		out.replace(index, rep.size(), repTo);

		last = index + rep.size();

		index = out.find_first_of(rep, last);
	}

	return out;
}

}
