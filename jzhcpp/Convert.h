#pragma once
#ifndef __HCODEC_HPP__
#define __HCODEC_HPP__

#include <windows.h>
#include <string>


// <to windows gbk>
inline std::wstring ATW(const std::string& str)
{
    std::wstring wstr;
    // <get lenth>
    auto len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wstr.resize(len);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, const_cast<wchar_t *>(wstr.c_str()), len);
    wstr.pop_back();

    return wstr;
}

inline std::string WTA(const std::wstring& wstr)
{
    std::string str;
    auto len = WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wstr.c_str(), -1, NULL, 0, NULL, NULL);
    str.resize(len);
    WideCharToMultiByte(CP_ACP, 0, (LPCWCH)wstr.c_str(), -1, (LPSTR)str.c_str(), len, NULL, NULL);
    str.pop_back();
    return str;
}

inline std::string WTU8(const std::wstring& strW)
{
    std::string strUtf8;
    auto len = WideCharToMultiByte(CP_UTF8, 0, strW.c_str(), -1, NULL, 0, NULL, NULL);
    strUtf8.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, strW.c_str(), -1, (LPSTR)strUtf8.c_str(), len, NULL, NULL);
    strUtf8.pop_back(); // <remove tail 0x00>

    return strUtf8;
}

inline std::wstring U8TW(const std::string& str)
{
    std::wstring wstr;
    auto len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wstr.resize(len);
    MultiByteToWideChar(CP_UTF8, 0, (LPCCH)str.c_str(), -1, (LPWSTR)wstr.c_str(), len);
    wstr.pop_back();

    return wstr;
}

inline std::string ATU8(const std::string& strGBK)
{
    return WTU8(ATW(strGBK));

}

// <UTF8 TO GBK>
inline std::string U8TA(const std::string& strUTF8)
{
    return WTA(U8TW(strUTF8));
}


#endif
