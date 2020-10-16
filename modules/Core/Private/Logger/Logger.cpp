/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <array>
#include <ctime>
#include <iostream>

#include <Netero/Logger.hpp>

namespace Netero {

Logger  StandardOuputLogger(std::cout);
Logger* DefaultGlobalLogger = &StandardOuputLogger;

static std::array<const char*, 4> LevelString { "DEBUG", "ERROR", "WARNING", "INFO" };

Logger::Logger(std::ostream& stream): _stream(&stream), _level(Level::Info)
{
}

const char* Logger::level_c_str(Level level) const
{
    return LevelString[static_cast<int>(level)];
}

void Logger::printPlaceholder() const
{
    time_t     rawtime;
    struct tm* timeinfo;
    char       buffer[32];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 32, "%a %d %b %G %T", timeinfo);

    *_stream << "(" << buffer << ") [" << this->level_c_str(this->_level) << "] ";
}

Logger& Log(Logger& logger, Level n)
{
    logger._level = n;
    return logger;
}

} // namespace Netero
