#pragma once

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using std::wstring;
using std::vector;
using std::map;

inline auto ReadStringListFromFileW(LPCWSTR filePath, TCHAR Separator = '\n')
{
	vector<wstring> retvec;

	std::wifstream ifs(filePath, std::ios::in);
	ifs.imbue(std::locale("ko-kr"));

	if (!ifs.is_open())
	{
		_DEBUG_ERROR("부적절한 경로이거나 잘못된 파일입니다!");
		ifs.close();
		return retvec;
	}

	wstring wstr;
	while (!ifs.eof())
	{
		wchar_t wc[1]{};
		ifs.read(wc, 1);
		wstr += wc[0];
		if (wc[0] == Separator)
		{
			wstr.pop_back();
			retvec.push_back(wstr);
			wstr.clear();
		}
	}

	if (wstr.length() > 0)
	{
		wstr.pop_back();
		retvec.push_back(wstr);
		wstr.clear();
	}
	while (true)
	{
		if (retvec.back().length() > 0) break;
		retvec.pop_back();
	}

	ifs.close();
	return retvec;
}

inline auto ReadStringListFromFileStoreByTagW(LPCWSTR filePath, TCHAR Separator = '\n', TCHAR beginTagSep = '[', TCHAR EndTagSep = ']')
{
	map<wstring, vector<wstring>> retmap;

	std::wifstream ifs(filePath, std::ios::in);
	ifs.imbue(std::locale("ko-kr"));

	if (!ifs.is_open())
	{
		_DEBUG_ERROR("부적절한 경로이거나 잘못된 파일입니다!");
		ifs.close();
		return retmap;
	}

	wstring wTag;
	wstring wstr;
	bool bTagStr = false;
	while (!ifs.eof())
	{
		wchar_t wc[2]{};
		ifs.read(wc, 1);
		if (wc[0] == beginTagSep)
		{
			bTagStr = true;
			wTag.clear();
			continue;
		}
		if (wc[0] == EndTagSep)
		{
			bTagStr = false;

			continue;
		}

		if (!bTagStr && wTag.empty()) continue;

		if (!bTagStr
			&& wstr.empty()
			&& (_istspace(wc[0]) > 0)
			) continue;

		((bTagStr) ? wTag : wstr) += wc[0];

		if (wc[0] == Separator)
		{
			wstr.pop_back();
			retmap[wTag].push_back(wstr);
			wstr.clear();
		}
	}

	if (!bTagStr && wTag.empty() && wstr.length() > 0)
	{
		wstr.pop_back();
		retmap[wTag].push_back(wstr);
		wstr.clear();
	}

	ifs.close();
	return retmap;
}

//inline auto ReadStringListFromFileStoreByTagWithAttributesW(LPCWSTR filePath, const vector<pair<TCHAR, TCHAR>>& vAttributeParser = { {'[',']'} }, TCHAR Separator = '\n', TCHAR Comment = '#')
//{
////	Depth 0 Tag					Attribute 		Strings
//	map<wstring,	vector<pair<set<wstring>,	wstring>>> retmap;
//
//	std::wifstream ifs(filePath, std::ios::in);
//	ifs.imbue(std::locale("ko-kr"));
//
//	if (!ifs.is_open())
//	{
//		_DEBUG_ERROR("부적절한 경로이거나 잘못된 파일입니다!");
//		ifs.close();
//		return retmap;
//	}
//
//	wstring wAttributeStr;
//	wstring wTagStr;
//	wstring wstr;
//	bool bAttributeStr = false;
//	bool bCommentLine = false;
//
//	size_t parseDepth = 0;
//
//	set<wstring> sAttribute;
//
//	while (!ifs.eof())
//	{
//parsing:
//		wchar_t wc[2]{};
//		ifs.read(wc, 1);
//
//		// comment 부분
//		if (wc[0] == Comment)
//		{
//			bCommentLine = true;
//			continue;
//		}
//		if (bCommentLine)
//		{
//			if (wc[0] == Separator)
//				bCommentLine = false;
//		}
//
//		if (!bAttributeStr)
//			for(size_t i = 0; i < vAttributeParser.size(); ++i)
//				if (wc[0] == vAttributeParser[i].first)
//				{
//					if (i == 0)
//					{
//						wTagStr.clear();
//						sAttribute.clear();
//					}
//					parseDepth = i;
//					bAttributeStr = true;
//					wAttributeStr.clear();
//					goto parsing;
//				}
//
//		if (bAttributeStr && wc[0] == vAttributeParser[parseDepth].second)
//		{
//			bAttributeStr = false;
//			if (parseDepth > 0)
//				sAttribute.insert(wAttributeStr);
//			continue;
//		}
//		
//		if (!bAttributeStr && wTagStr.empty()) continue;
//		
//		if (!bAttributeStr 
//			&& wstr.empty()
//			&& (_istspace(wc[0]) > 0)
//			) continue;
//
//		if (bAttributeStr)
//		{
//			if (parseDepth == 0)
//				wTagStr += wc[0];
//			else wAttributeStr += wc[0];
//		}
//		else wstr += wc[0];
//
//		if (wc[0] == Separator)
//		{
//			wstr.pop_back();
//			retmap[wTagStr].emplace_back(make_pair(sAttribute, wstr));
//			wstr.clear();
//		}
//	}
//
//	if (!bAttributeStr && wAttributeStr.empty() && wstr.length() > 0)
//	{
//		wstr.pop_back();
//		retmap[wTagStr].emplace_back(make_pair(sAttribute, wstr));
//		wstr.clear();
//	}
//
//	ifs.close();
//	return retmap;
//}
//