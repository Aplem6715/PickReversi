#include "logger.h"
#include <iomanip>
#include <iostream>

namespace game
{
    void Logger::Log(LogLevel level, const std::string& message)
    {
        buffer_.emplace_back(LogData{level, message, std::chrono::system_clock::now()});
    }

    void Logger::Flush(bool show_time)
    {
        for (const auto& log : buffer_)
        {
            if (show_time)
            {
                auto time = std::chrono::system_clock::to_time_t(log.time_point);
                std::cout << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] ";
            }
            std::cout << "[" << LogLevelToString(log.level) << "] " << log.message << std::endl;
        }
        buffer_.clear();
    }

    std::string Logger::LogLevelToString(LogLevel level) const
    {
        switch (level)
        {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        default:
            return "";
        }
    }
}
