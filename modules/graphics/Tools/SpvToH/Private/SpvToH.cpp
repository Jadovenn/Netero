/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include "SpvToH.hpp"

#include <Netero/ProgramArguments.hpp>
#include <netero/logger.hpp>

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

void ConfigureProgramArguments(Netero::Console::ProgramArguments& someProgramArguments)
{
    someProgramArguments.AddOption(OPTION_HELP, OPTION_HELP_DETAILED);

    someProgramArguments.AddArgument(Netero::Console::ProgramArgumentType::OPTIONS, OPTION, "");
    someProgramArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                     SHADER_NAME,
                                     "[^:]+");
    someProgramArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                     INPUT_PATH,
                                     "[^:]+(.spv)");
    someProgramArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                     OUTPUT_PATH,
                                     "[^:]+(.h)");
}

int main(int argc, const char** argv)
{
    Netero::Console::ProgramArguments programArguments;
    ConfigureProgramArguments(programArguments);
    programArguments.Parse(argc, argv);

    if (programArguments.GetAsBool(OPTION_HELP)) {
        ShowUsage(programArguments.ProgramName());
        return 0;
    }
    if (programArguments.UnexpectedCount() || !programArguments.MissingArgs().empty()) {
        const auto& missingArgs = programArguments.MissingArgs();
        for (const auto& missingArg : missingArgs) {
            LOG << "Missing: " << missingArg << std::endl;
        }
        LOG << "" << std::endl;
        ShowUsage(programArguments.ProgramName());
        return 1;
    }

    auto*             input_file_path = programArguments.GetAsStrings(INPUT_PATH);
    std::ifstream     input_file(input_file_path->front(), std::ios::ate | std::ios::binary);
    std::vector<char> input_file_data;
    unsigned          input_word_count = 0;

    if (input_file.is_open()) {
        SpvToH::ExtractFileDataAsChar(input_file, input_file_data);
        input_word_count = SpvToH::ExtractArrayCount(input_file_data);
        input_file.close();
    }
    else {
        LOG_ERROR << "Could not open " << input_file_path->front() << std::endl;
        return 1;
    }

    auto* symbol_suffix = programArguments.GetAsStrings(SHADER_NAME);
    auto* output_file_path = programArguments.GetAsStrings(OUTPUT_PATH);
    auto  output_file_name = SpvToH::ExtractFileNameWithoutExtension(output_file_path->front());
    std::ofstream output_file(output_file_path->front(), std::ios::trunc);

    if (output_file.is_open()) {
        SpvToH::PrintNeteroHeader(output_file);
        SpvToH::PrintHeaderGuardPrefix(output_file, output_file_name);
        SpvToH::EmitSymbols(output_file, symbol_suffix->front(), input_file_data, input_word_count);
        SpvToH::PrintHeaderGuardSuffix(output_file, output_file_name);
        output_file.close();
    }
    else {
        LOG_ERROR << "Could not open " << output_file_path->front() << std::endl;
        return 1;
    }
    return 0;
}
