///=====================================================/// 
/// 
///ログの出力
/// 
///=====================================================///
#pragma once
#include <string>
#include <Windows.h>

namespace Logger {
	enum class LogLevel {
		Info,
		Warning,
		Error
	};

	void Log(const std::string &message, LogLevel level = LogLevel::Info);
}
