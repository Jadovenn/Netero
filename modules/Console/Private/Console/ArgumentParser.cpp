/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Console/ArgumentsParser.hpp>
#include <Netero/Logger.hpp>

namespace Netero::Console {

void ArgumentsParser::Execute(int argc, const char** argv)
{
    unsigned argument_state_idx = 0;
    myProgramName = argv[0];
    myMissingArguments.clear();
    myUnexpectedArguments.clear();

    for (unsigned idx = 1; idx < argc;) {
        unsigned result = ParserState::IDLE;

        if (argv[idx][0] == '-') {
            for (auto& option : myOptions) {
                result = option->Parse(argv[idx]);
                if (result & ParserState::CONSUMED) {
                    break;
                }
            }
            if (result & ParserState::ERROR) {
                result = ParserState::UNEXPECTED_ARGUMENT | ParserState::CONSUMED;
            }
        }
        else if (argument_state_idx < myPositionalArguments.size()) {
            auto& argument = myPositionalArguments[argument_state_idx];
            result = argument->Parse(argv[idx]);
            if (result & ParserState::ERROR) {
                myUnexpectedArguments.emplace_back(argument->GetName(), argv[idx]);
            }
        }
        else {
            result = ParserState::UNEXPECTED_ARGUMENT | ParserState::CONSUMED;
        }

        if (result & ParserState::UNEXPECTED_ARGUMENT) {
            myUnexpectedArguments.emplace_back("", argv[idx]);
        }
        if (result & ParserState::PUSH_STATE) {
            argument_state_idx += 1;
        }
        if (result & ParserState::MANDATORY_ARGUMENT) {
            myCount += 1;
        }
        if (result & ParserState::CONSUMED) {
            idx += 1;
        }
    }
    for (unsigned idx = argument_state_idx; idx < myPositionalArguments.size(); ++idx) {
        auto& argument = myArguments[idx];
        myMissingArguments.emplace_back(argument->GetName());
    }
}

void ArgumentsParser::ShowUsage()
{
    LOG << "Usage: " << GetProgramName();
    if (!myOptions.empty()) {
        LOG << " [OPTION]";
    }
    for (auto& argument : myPositionalArguments) {
        LOG << " " << argument->GetName();
    }
    LOG << Endl;
    if (!myDescription.empty()) {
        LOG << myDescription << std::endl;
    }
    LOG << Endl;
    size_t biggestSize = 0;
    for (auto& argument : myArguments) {
        size_t size = argument.second->GetName().size();
        if (biggestSize < size) {
            biggestSize = size;
        }
    }
    biggestSize += 4;
    if (!myPositionalArguments.empty()) {
        LOG << "Arguments:" << std::endl;
    }
    for (auto& argument : myPositionalArguments) {
        auto name = argument->GetName();
        auto description = argument->GetDescription();
        while (name.size() < biggestSize) {
            name += " ";
        }
        LOG << name << description << std::endl;
    }

    if (!myOptions.empty()) {
        LOG << Endl;
        LOG << "Options:" << std::endl;
    }
    for (auto& argument : myOptions) {
        auto name = argument->GetName();
        auto description = argument->GetDescription();
        while (name.size() < biggestSize) {
            name += " ";
        }
        LOG << name << description << std::endl;
    }
    if (!myFooter.empty()) {
        LOG << Endl;
        LOG << myFooter;
    }
}

} // namespace Netero::Console
