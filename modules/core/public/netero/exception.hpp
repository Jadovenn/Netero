/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <string>

namespace netero {

class exception {
    public:
    exception(const std::string& message) noexcept: _message(message) {}

    const char* what() const noexcept { return _message.c_str(); }

    private:
    const std::string _message;
};

namespace exceptions {

    class memory_exhausted: public exception {
        public:
        memory_exhausted(): exception("Memory exhausted can not continue execution.") {}
    };

    class bad_slot_call: public exception {
        public:
        bad_slot_call(): exception("Bad slot call.") {}
    };

} // namespace exceptions

} // namespace netero
