///=====================================================/// 
/// 
///ログの出力
/// 
///=====================================================///

#include "Logger.h"

namespace Logger {
	void Log(const std::string& message) {
		OutputDebugStringA(message.c_str());
	}
}