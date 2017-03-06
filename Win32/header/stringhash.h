#pragma once

using string_hash = size_t;

namespace LusHashStringInnerRangeNameSpace
{
	constexpr inline string_hash GetLastStringHashValue(string_hash val) noexcept
	{
		return static_cast<string_hash>(val ^ (val >> 16));
	}

	constexpr inline string_hash GeStringtHash(const char* Str, const size_t n, const size_t N) noexcept
	{
		return	  n == 0 ? Str[n]
				: n == N ? GetLastStringHashValue(GeStringtHash(Str, n - 1, N))
				: static_cast<string_hash>((GeStringtHash(Str, n - 1, N) * 65599) + (128 + Str[n]));
	}
};

constexpr inline string_hash operator "" _hash(const char* _Str, size_t n) noexcept
{
	using LusHashStringInnerRangeNameSpace::GeStringtHash;
	return (GeStringtHash(_Str, n, n));
}

template<typename str, size_t N>
constexpr inline string_hash GeStringtHash(str(&Str)[N], const size_t n = N) noexcept
{
	using LusHashStringInnerRangeNameSpace::GeStringtHash;
	return (GeStringtHash(Str, n, n));
}

template<typename str>
constexpr inline string_hash GeStringtHash(const str *Str, const size_t n) noexcept
{
	using LusHashStringInnerRangeNameSpace::GeStringtHash;
	return (GeStringtHash(Str, n, n));
}

inline string_hash GeStringtHash(string&& Str) noexcept
{
	using LusHashStringInnerRangeNameSpace::GeStringtHash;
	return (GeStringtHash(Str.c_str(), Str.length(), Str.length()));
}

inline string_hash GeStringtHash(const string& Str) noexcept
{
	using LusHashStringInnerRangeNameSpace::GeStringtHash;
	return (GeStringtHash(Str.c_str(), Str.length(), Str.length()));
}

//template<typename str>
//constexpr inline string_hash GetStringHash(str&& Str, true_type) noexcept
//{
//	using LusHashStringInnerRangeNameSpace::GeStringtHash;
//	return (GeStringtHash(Str.c_str(), Str.length(), Str.length()));
//}
//template<typename str>
//constexpr inline string_hash GeStringtHash(str&& Str) noexcept
//{
//	using unref_str = remove_const_t<remove_reference_t<str>>;
//	return GetStringHash(forward<str>(Str), is_same<true_type, bool_constant<is_same<unref_str, string>::value || is_same<unref_str, wstring>::value>>{});
//}
