/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <regex>
#include <unordered_map>
#include <vector>

namespace Netero::Console {

enum ProgramArgumentType {
    OPTIONS = 0, /* OPTIONS, lookup for options until first arg without '-' founded */
    ARGUMENT,    /* ARGUMENT, a basic non optional argument */
};

class ProgramArguments {
    public:
    ProgramArguments(): myUnexpectedCount(0), myCount(0), myProgramName(nullptr) {}

    ProgramArguments(const ProgramArguments&) = delete;
    ProgramArguments(ProgramArguments&&) = delete;
    ProgramArguments& operator=(const ProgramArguments&) = delete;
    ProgramArguments& operator=(ProgramArguments&&) = delete;

    void AddOption(const std::string& anOptionName, const std::string& aVariant)
    {
        myOptions.emplace_back(anOptionName, aVariant);
    }

    void AddArgument(ProgramArgumentType anArgumentType,
                     const std::string&  anArgumentName,
                     const std::string&  regex)
    {
        myArguments.emplace_back(
            std::make_tuple(anArgumentType, anArgumentName, std::regex { regex }));
    }

    void clear()
    {
        myOptions.clear();
        myArguments.clear();
        myResults.clear();
        myMissingArgs.clear();
        myProgramName = nullptr;
        myCount = 0;
        myUnexpectedCount = 0;
    }

    void Parse(const int argc, const char** argv) { ParserExecute(argc, argv); }

    [[nodiscard]] unsigned UnexpectedCount() const { return myUnexpectedCount; }

    [[nodiscard]] unsigned Count() const { return myCount; }

    [[nodiscard]] const char* ProgramName() const { return myProgramName; }

    [[nodiscard]] const std::vector<std::string>& MissingArgs() const { return myMissingArgs; }

    bool GetAsBool(const std::string& argument)
    {
        auto it = myResults.find(argument);
        if (it != myResults.end()) {
            return true;
        }
        return false;
    }

    std::vector<std::string>* GetAsStrings(const std::string& argument)
    {
        auto it = myResults.find(argument);
        if (it != myResults.end()) {
            return &it->second;
        }
        return nullptr;
    }

    private:
    /* Argument type, ProgramArgumentType, argument name, argument corresponding regex */
    using Argument = std::tuple<ProgramArgumentType, std::string, std::regex>;
    /* Option name, associated with it variant */
    using Option = std::pair<std::string, std::string>;

    /* Arguments declared by the client. Ordered first to last for right to left */
    std::vector<Argument> myArguments;
    /* Option to lookup for options arguments */
    std::vector<Option> myOptions;
    /* Result after parsing. Client can look it up as much as he want */
    std::unordered_map<std::string, std::vector<std::string>> myResults;

    enum ParserState : unsigned {
        IDLE = 0x00,                /* Dummy default value */
        CONSUMED = 0x01,            /* Input has been consumed */
        PUSH_STATE = 0x02,          /* Parser should go to next state */
        MANDATORY_ARGUMENT = 0x04,  /* Increment mandatory arg count  */
        UNEXPECTED_ARGUMENT = 0x08, /* Print argument error */
        ERROR = 0x10,               /* An error occurred */
    };

    std::vector<std::string> myMissingArgs;     /* Missing arguments accumulator */
    unsigned                 myUnexpectedCount; /* Unexpected arguments count */
    unsigned                 myCount; /* Arguments count, careful this does not include options */
    const char*              myProgramName; /* program name from argv[0] */

    void                   ParserExecute(int argc, const char** argv);
    [[nodiscard]] unsigned ParserConsumeInput(const char* input, Argument& argument);
    [[nodiscard]] unsigned ParserReadOption(const char* input);
    [[nodiscard]] unsigned ParserReadArgument(const char* input, Argument& argument);
};

} // namespace Netero::Console
