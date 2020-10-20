/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "SpvToH.hpp"

#include <Netero/Console/ArgumentsParser.hpp>
#include <Netero/Logger.hpp>

#include "SpvToHArguments.hpp"

constexpr char const* OPTION_HELP = "-h";
constexpr char const* OPTION_HELP_DETAILED = "--help";
constexpr char const* OPTION = "[OPTION]";
constexpr char const* SHADER_NAME = "<shader_name>";
constexpr char const* INPUT_PATH = "<input_path>";
constexpr char const* OUTPUT_PATH = "<output_path>";

void ShowUsage(const char* aProgramName)
{
    LOG << "Usage: " << aProgramName << " <shader_name> <input_path> <output_path>" << std::endl;
    LOG << "Translate a precompiled spv's shader formatted in c to a valid .h header file."
        << std::endl;
    LOG << "" << std::endl;
    LOG << "Options:" << std::endl;
    LOG << "-h, --help    Show this help." << std::endl;
    LOG << "" << std::endl;
    LOG << "Arguments:" << std::endl;
    LOG << "" << std::endl;
    LOG << "<shader_name>    Shader's name to emit <shader_name>Data and <shader_name>DataCount symbols"
        << std::endl;
    LOG << "<input_path>     .c_spv input file path." << std::endl;
    LOG << "<output_path>    .h output file path." << std::endl;
    LOG << "" << std::endl;
    LOG << "Example:" << std::endl;
    LOG << aProgramName << " DefaultVertexShader DefaultVertexShader.c_spv DefaultVertexShader.h"
        << std::endl;
}

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
