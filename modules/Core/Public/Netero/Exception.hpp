/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>

namespace Netero {

class Exception {
    public:
    Exception(const std::string& message) noexcept: _message(message) {}

    const char* what() const noexcept { return _message.c_str(); }

    private:
    const std::string _message;
};

namespace Exceptions {

    class MemoryExausted: public Exception {
        public:
        MemoryExausted(): Exception("Memory exhausted can not continue execution.") {}
    };

    class BadSlotCall: public Exception {
        public:
        BadSlotCall(): Exception("Bad slot call.") {}
    };

} // namespace Exceptions

} // namespace Netero
