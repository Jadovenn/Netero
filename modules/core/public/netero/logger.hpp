/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <ostream>

namespace netero {

enum class Level { debug = 0, error = 1, warning = 2, info = 3, raw = 4 };

class Logger {
    protected:
    std::ostream* _stream;
    Level         _level;

    const char*  level_c_str(Level) const;
    virtual void printPlaceholder() const;

    public:
    explicit Logger(std::ostream& stream);

    template<typename T>
    std::ostream& operator<<(T const& object)
    {
        if (this->_level != Level::raw) {
            this->printPlaceholder();
        }
        *this->_stream << object;
        return *this->_stream;
    }

    friend Logger& log(Logger& logger, Level n);
};

Logger& log(Logger& logger, Level n);
extern Logger* DefaultGlobalLogger;

} // namespace netero

#define LOG      netero::log(*netero::DefaultGlobalLogger, netero::Level::raw)
#define LOG_INFO netero::log(*netero::DefaultGlobalLogger, netero::Level::info)
#define LOG_DEBUG                                                                                  \
    netero::log(*netero::DefaultGlobalLogger, netero::Level::debug)                                \
        << '{' << __FILE__ << " l. " << __LINE__ << "} "
#define LOG_WARNING netero::log(*netero::DefaultGlobalLogger, netero::Level::warning)
#define LOG_ERROR   netero::log(*netero::DefaultGlobalLogger, netero::Level::error)
