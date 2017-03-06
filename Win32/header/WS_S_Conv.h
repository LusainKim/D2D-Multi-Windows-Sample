#pragma once

#include <codecvt>
#include <locale>
#include <string>

/// <summary>
/// std::string 객체의 문자열을 std::wstring 객체의 문자열로 변환합니다.
/// </summary>
///	<param name = "str"> 변환할 std::string 객체입니다. </param>
///	<param name = "Loc"> 변환할 문자집합의 locale 값입니다. 한국은 locale("ko") 를 사용합니다. </param>
inline std::wstring s_to_ws(const std::string& str, std::locale Loc = std::locale("ko"))
{
	using convert_typeX = std::codecvt_utf8_utf16<wchar_t>;
	return std::wstring_convert<convert_typeX, wchar_t>
	{ &std::use_facet<convert_typeX>(Loc) }.from_bytes(str);
}

/// <summary>
/// std::wstring 객체의 문자열을 std::string 객체의 문자열로 변환합니다.
/// </summary>
///	<param name = "str"> 변환할 std::wstring 객체입니다. </param>
///	<param name = "Loc"> 변환할 문자집합의 locale 값입니다. 한국은 locale("ko") 를 사용합니다. </param>
inline std::string ws_to_s(const std::wstring& wstr, std::locale Loc = std::locale("ko"))
{
	using conv_Ty = std::codecvt_utf8_utf16<wchar_t>;
	return std::wstring_convert<conv_Ty, wchar_t>
	{ &std::use_facet<conv_Ty>(Loc) }.to_bytes(wstr);
}
