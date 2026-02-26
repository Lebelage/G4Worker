#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>

namespace G4Worker::Utils
{
    enum class LogLevel
    {
        Success,
        Warning,
        Error
    };

    class Logger
    {
    public:
        template <typename... Args>
        static void Log(LogLevel level, Args &&...args)
        {
            std::ostringstream ss;
            (ss << ... << args);

            Print(level, ss.str());
        }

    private:
        static std::string GetTimeString()
        {
            using namespace std::chrono;

            auto now = system_clock::now();
            auto time_t_now = system_clock::to_time_t(now);

            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &time_t_now);
#else
            localtime_r(&time_t_now, &tm);
#endif

            std::ostringstream ss;
            ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }

        static void Print(LogLevel level, const std::string &message)
        {
            std::string prefix;
            std::string color;

            switch (level)
            {
            case LogLevel::Success:
                prefix = "SUCCESS";
                color = "\033[32m";
                break;

            case LogLevel::Warning:
                prefix = "WARNING";
                color = "\033[33m";
                break;

            case LogLevel::Error:
                prefix = "ERROR";
                color = "\033[31m";
                break;
            }

            const std::string reset = "\033[0m";

            std::cout
                << color
                << "[" << GetTimeString() << "] "
                << "[" << prefix << "] "
                << message
                << reset
                << std::endl;
        }
    };

}