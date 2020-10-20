/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <regex>
#include <string>

namespace Netero::Console {

enum ParserState : unsigned {
    IDLE = 0x00,                /* Dummy default value */
    CONSUMED = 0x01,            /* Input has been consumed */
    PUSH_STATE = 0x02,          /* Parser should go to next state */
    MANDATORY_ARGUMENT = 0x04,  /* Increment mandatory arg count  */
    UNEXPECTED_ARGUMENT = 0x08, /* Print argument error */
    ERROR = 0x10,               /* An error occurred */
};

class Argument {
    public:
    explicit Argument(bool aPresentFlag = false): myIsPresent(aPresentFlag) {}
    virtual ~Argument() = default;

    virtual bool IsPresent() { return myIsPresent; }

    virtual const std::string GetName() = 0;

    virtual const std::string GetDescription() = 0;

    virtual unsigned Parse(const char* aToken) = 0;

    protected:
    bool myIsPresent;
};

template<typename T = std::string>
class PositionalArgument: public Argument {
    public:
    typedef T ArgumentType;
    ~PositionalArgument() override = default;

    unsigned Parse(const char* aToken) final
    {
        std::cmatch match;
        std::regex_search(aToken, match, GetRegex());
        if (match.empty()) {
            myIsPresent = false;
            return ParserState::ERROR | ParserState::PUSH_STATE | ParserState::CONSUMED;
        }
        myIsPresent = AssignMatchResult(match);
        if (!myIsPresent) {
            return ParserState::ERROR | ParserState::PUSH_STATE;
        }
        return ParserState::PUSH_STATE | ParserState::CONSUMED | ParserState::MANDATORY_ARGUMENT;
    }

    virtual bool AssignMatchResult(std::cmatch& aMatchResult);

    ArgumentType Get() { return myArgumentValue; }

    protected:
    virtual const std::regex GetRegex() = 0;

    private:
    ArgumentType myArgumentValue;
};

template<>
bool PositionalArgument<std::string>::AssignMatchResult(std::cmatch& aMatchResult);

template<>
bool PositionalArgument<int>::AssignMatchResult(std::cmatch& aMatchResult);

class Option: public Argument {
    public:
    virtual ~Option() = default;
    typedef bool ArgumentType;

    unsigned Parse(const char* aToken) final
    {
        if (aToken[0] != '-') {
            return ParserState::PUSH_STATE;
        }
        auto option = GetOptionVariant();
        if (option.first == aToken || option.second == aToken) {
            myIsPresent = true;
            return ParserState::CONSUMED;
        }
        return ParserState::ERROR;
    }

    virtual const std::pair<std::string, std::string> GetOptionVariant() = 0;

    bool Get() { return myIsPresent; }
};

} // namespace Netero::Console
