/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "SpvToH.hpp"

#include <Netero/Console/ArgumentsParser.hpp>
#include <Netero/Logger.hpp>

#include "SpvToHArguments.hpp"

void ConfigureProgramArguments(Netero::Console::ArgumentsParser& anArgumentsParser)
{
    anArgumentsParser.SetDescription(
        "Translate a precompiled spv's shader formatted in c to a valid .h header file.");
    anArgumentsParser.AddArgument<HelpOption>();
    anArgumentsParser.AddArgument<ShaderNameArgument>();
    anArgumentsParser.AddArgument<InputPathArgument>();
    anArgumentsParser.AddArgument<OutputPathArgument>();
    anArgumentsParser.SetFooter(
        "Example:\nSpvToH DefaultVertexShader DefaultVertexShader.c_spv DefaultVertexShader.h");
}

int main(int argc, const char** argv)
{
    Netero::Console::ArgumentsParser argumentsParser;
    ConfigureProgramArguments(argumentsParser);
    argumentsParser.Execute(argc, argv);

    if (argumentsParser.IsPresent<HelpOption>()) {
        argumentsParser.ShowUsage();
        return 0;
    }
    if (argumentsParser.GetUnexpectedArgumentsCount() ||
        argumentsParser.GetMissingArgumentsCount()) {
        const auto& unexpectedArgs = argumentsParser.GetUnexpectedArguments();
        for (const auto& unexpectedArg : unexpectedArgs) {
            LOG << "Unexpected";
            if (!unexpectedArg.first.empty()) {
                LOG << " " << unexpectedArg.first;
            }
            LOG << " : " << unexpectedArg.second << std::endl;
        }
        const auto& missingArgs = argumentsParser.GetMissingArguments();
        for (const auto& missingArg : missingArgs) {
            LOG << "Missing: " << missingArg << std::endl;
        }
        argumentsParser.ShowUsage();
        return 1;
    }

    auto              input_file_path = argumentsParser.Get<InputPathArgument>();
    std::ifstream     input_file(input_file_path, std::ios::ate | std::ios::binary);
    std::vector<char> input_file_data;
    unsigned          input_word_count = 0;

    if (input_file.is_open()) {
        SpvToH::ExtractFileDataAsChar(input_file, input_file_data);
        input_word_count = SpvToH::ExtractArrayCount(input_file_data);
        input_file.close();
    }
    else {
        LOG_ERROR << "Could not open " << input_file_path << std::endl;
        return 1;
    }

    auto          symbol_suffix = argumentsParser.Get<ShaderNameArgument>();
    auto          output_file_path = argumentsParser.Get<OutputPathArgument>();
    auto          output_file_name = SpvToH::ExtractFileNameWithoutExtension(output_file_path);
    std::ofstream output_file(output_file_path, std::ios::trunc);

    if (output_file.is_open()) {
        SpvToH::PrintNeteroHeader(output_file);
        SpvToH::PrintHeaderGuardPrefix(output_file, output_file_name);
        SpvToH::EmitSymbols(output_file, symbol_suffix, input_file_data, input_word_count);
        SpvToH::PrintHeaderGuardSuffix(output_file, output_file_name);
        output_file.close();
    }
    else {
        LOG_ERROR << "Could not open " << output_file_path << std::endl;
        return 1;
    }
    return 0;
}
