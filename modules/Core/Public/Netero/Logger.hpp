/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <ostream>

namespace Netero {

enum class Level { Debug = 0, Error = 1, Warning = 2, Info = 3, Raw = 4 };

class Logger {
    protected:
    std::ostream* _stream;
    Level         _level;

    [[nodiscard]] const char* level_c_str(Level) const;
    virtual void              printPlaceholder() const;

    public:
    explicit Logger(std::ostream& stream);

    template<typename T>
    std::ostream& operator<<(T const& object)
    {
        if (this->_level != Level::Raw) {
            this->printPlaceholder();
        }
        *this->_stream << object;
        return *this->_stream;
    }

    friend Logger& Log(Logger& logger, Level n);
};

Logger&        Log(Logger& logger, Level n);
extern Logger* DefaultGlobalLogger;

} // namespace Netero

#define LOG      Netero::Log(*Netero::DefaultGlobalLogger, Netero::Level::Raw)
#define LOG_INFO Netero::Log(*Netero::DefaultGlobalLogger, Netero::Level::Info)
#define LOG_DEBUG                                                                                  \
    Netero::Log(*Netero::DefaultGlobalLogger, Netero::Level::Debug)                                \
        << '{' << __FILE__ << " l. " << __LINE__ << "} "
#define LOG_WARNING Netero::Log(*Netero::DefaultGlobalLogger, Netero::Level::Warning)
#define LOG_ERROR   Netero::Log(*Netero::DefaultGlobalLogger, Netero::Level::Error)
