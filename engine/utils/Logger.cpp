#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Logger {

    void Log(const std::string& message, LogLevel level) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        std::string levelStr;
		WORD color = 7; // White

        switch (level) {
        case LogLevel::Info:
            levelStr = "[INFO] ";
            color = 7; // White
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

        std::string logMessage = levelStr + message + "\n";
        SetConsoleTextAttribute(hConsole, color);
        OutputDebugStringA(logMessage.c_str());
        std::cout << logMessage;
        SetConsoleTextAttribute(hConsole, 7); // Reset to default color
    }
}
