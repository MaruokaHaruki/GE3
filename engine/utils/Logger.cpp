#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Logger {

    /**----------------------------------------------------------------------------
     * @brief 現在の時刻を取得する関数
     * @return std::string 現在の時刻をフォーマットした文字列
     */
    std::string GetCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm timeInfo;
        localtime_s(&timeInfo, &in_time_t);
        std::stringstream ss;
        ss << std::put_time(&timeInfo, "%Y-%m-%d %X");
        return ss.str();
    }

    /**----------------------------------------------------------------------------
     * @brief ログメッセージを出力する関数
     * @param message ログメッセージ
     * @param level ログレベル (Info, Warning, Error)
     */
    void Log(const std::string &message, LogLevel level) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        std::string levelStr;
        WORD color = 7; // White

        // ログレベルに応じて文字列と色を設定
        switch(level) {
        case LogLevel::Info:
            levelStr = "[INFO] ";
            color = 7; // White
            break;
		case LogLevel::Success:
			levelStr = "[SUCCESS] ";
			color = 10; // Green
			break;
        case LogLevel::Warning:
            levelStr = "[WARNING] ";
            color = 14; // Yellow
            break;
        case LogLevel::Error:
            levelStr = "[ERROR] ";
            color = 12; // Red
            break;
        }

        // ログメッセージをフォーマット
        std::string logMessage = levelStr + GetCurrentTime() + " : " + message + "\n";
        // コンソールの文字色を設定
        SetConsoleTextAttribute(hConsole, color);
        // デバッグ出力と標準出力にログメッセージを出力
        OutputDebugStringA(logMessage.c_str());
        std::cout << logMessage;
        // コンソールの文字色をデフォルトに戻す
        SetConsoleTextAttribute(hConsole, 7);
    }

    /**----------------------------------------------------------------------------
     * @brief 情報レベルのログメッセージを出力する関数
     * @param message ログメッセージ
     */
    void LogInfo(const std::string &message) {
        Log(message, LogLevel::Info);
    }

    /**----------------------------------------------------------------------------
     * @brief 警告レベルのログメッセージを出力する関数
     * @param message ログメッセージ
     */
    void LogWarning(const std::string &message) {
        Log(message, LogLevel::Warning);
    }

    /**----------------------------------------------------------------------------
     * @brief エラーレベルのログメッセージを出力する関数
     * @param message ログメッセージ
     */
    void LogError(const std::string &message) {
        Log(message, LogLevel::Error);
    }
}
