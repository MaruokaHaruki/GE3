///=====================================================/// 
/// 
///ログの出力
/// 
///=====================================================///

#pragma once
#include <string>
#include <Windows.h>

namespace Logger {
	///ログの生成
	void Log(const std::string& message);
}