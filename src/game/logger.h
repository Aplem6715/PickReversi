#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <string>
#include <vector>

namespace game
{
    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error
    };

    struct LogData
    {
        LogLevel level;
        std::string message;
        std::chrono::system_clock::time_point time_point;
    };

    class Logger
    {
    public:
        void Log(LogLevel level, const std::string& message);
        void Flush(bool show_time = true);

        void Debug(const std::string& message) { Log(LogLevel::Debug, message); }
        void Info(const std::string& message) { Log(LogLevel::Info, message); }
        void Warning(const std::string& message) { Log(LogLevel::Warning, message); }
        void Error(const std::string& message) { Log(LogLevel::Error, message); }

    private:
        std::vector<LogData> buffer_;
        std::string LogLevelToString(LogLevel level) const;
    };
}

#endif