#include "character.h"
#include <iostream>

bool ischar(char t) {
	return ('*' == t);
}

int main() {

	std::string str = "  a";
	std::string t = Character::Trim::trim_copy_if(str, Character::Trim::is_space);

	std::cout << Character::StrIsEmpty(str) << std::endl;

	auto constr = Character::StrConcat("ÄãºÃ", "±±¾©");
	std::cout << constr << std::endl;

	std::string xing = "*******hello****";
	auto ax = Character::Trim::trim_copy_if(xing, ischar);
	std::cout << ax << std::endl;

	Character::Trim::trim_if(xing, ischar);
	std::cout << xing << std::endl;

	std::string splitStr = "aaaa,bbb,ccc,dddgggg";
	auto list = Character::StrSplit(splitStr, ",");

	std::vector<std::string> v = {"123", "456", "788"};
	auto join = Character::StrJoin(v, "-");

	std::string lu = "abcdERTQTagadgWEWQET";
	std::cout << Character::StrToLower(lu) << std::endl;
	std::cout << Character::StrToUpper(lu) << std::endl;

	std::string repstr = "1242qq241242qq41321qq123qqqqqq";
	auto rep = Character::StrReplace(repstr, "qq", "AA");

	getchar();
	return 0;
}