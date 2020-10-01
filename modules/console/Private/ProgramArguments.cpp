/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <netero/logger.hpp>
#include <Netero/ProgramArguments.hpp>

namespace Netero::Console {

void ProgramArguments::ParserExecute(const int argc, const char** argv)
{
    unsigned argument_state_idx = 0;
    myProgramName = argv[0];
    myCount = 0;
    myUnexpectedCount = 0;
    myResults.clear();
    myMissingArgs.clear();

    for (unsigned idx = 1; idx < argc;) {
        unsigned result = ParserState::IDLE;

        if (argument_state_idx < myArguments.size()) {
            auto& argument = myArguments[argument_state_idx];
            result = ParserConsumeInput(argv[idx], argument);
            if (result & ParserState::ERROR) {
                LOG << "Unexpected " << std::get<1>(argument) << ": " << argv[idx]
                          << std::endl;
                myUnexpectedCount += 1;
            }
        }
        else {
            result = ParserState::UNEXPECTED_ARGUMENT | ParserState::CONSUMED;
        }

        if (result & ParserState::UNEXPECTED_ARGUMENT) {
            LOG << "Unexpected argument: " << argv[idx] << std::endl;
            myUnexpectedCount += 1;
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
    for (unsigned idx = argument_state_idx; idx < myArguments.size(); ++idx) {
        auto& argument = myArguments[idx];
        myMissingArgs.push_back(std::get<1>(argument));
    }
}

unsigned ProgramArguments::ParserConsumeInput(const char* input, Argument& argument)
{
    if (std::get<0>(argument) == ProgramArgumentType::OPTIONS) {
        return ParserReadOption(input);
    }
    else { // No other option than argument parsing
        return ParserReadArgument(input, argument);
    }
}

unsigned ProgramArguments::ParserReadOption(const char* input)
{
    if (input[0] != '-') {
        return ParserState::PUSH_STATE;
    }
    for (auto& option : myOptions) {
        if (option.first == input || option.second == input) {
            myResults.emplace(option.first, std::vector<std::string> { "true" });
            return ParserState::CONSUMED;
        }
    }
    return ParserState::CONSUMED | ParserState::ERROR;
}

unsigned ProgramArguments::ParserReadArgument(const char* input, Argument& argument)
{
    if (input[0] == '-') {
        return ParserState::ERROR | ParserState::CONSUMED;
    }
    std::cmatch match;
    std::regex_search(input, match, std::get<2>(argument));
    if (match.empty()) {
        return ParserState::ERROR | ParserState::PUSH_STATE | ParserState::CONSUMED;
    }
    std::vector<std::string> string_result(match.begin(), match.end());
    myResults.emplace(std::get<1>(argument), string_result);
    return ParserState::PUSH_STATE | ParserState::CONSUMED | ParserState::MANDATORY_ARGUMENT;
}

} // namespace Netero::Console
