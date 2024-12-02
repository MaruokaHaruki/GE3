///=====================================================/// 
/// 
///文字列変換
/// 
///=====================================================///

#pragma once
#include <Windows.h>
#include <string>

namespace WstringUtility {
	//Wstring -> string
	std::wstring ConvertString(const std::string& str);

	//string -> Wstring
	std::string ConvertString(const std::wstring& str);
}