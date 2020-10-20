/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Console/Arguments.hpp>

class HelpOption final: public Netero::Console::Option {
    public:
    const std::string GetName() final { return "-h, --help"; }

    const std::string GetDescription() final { return "Show this help."; }

    protected:
    const std::pair<std::string, std::string> GetOptionVariant() final
    {
        return { "-h", "--help" };
    }
};

class ShaderNameArgument final: public Netero::Console::PositionalArgument<std::string> {
    public:
    const std::string GetName() final { return "<shader_name>"; }

    const std::string GetDescription() final
    {
        return "Shader's name to emit<shader_name>Data and <shader_name>DataCount symbols.";
    }

    protected:
    const std::regex GetRegex() final { return std::regex(R"([^:]+)"); }
};

class InputPathArgument final: public Netero::Console::PositionalArgument<std::string> {
    public:
    const std::string GetName() final { return "<input_path>"; }

    const std::string GetDescription() final
    {
        return ".spv input file path formatted in C style array.";
    }

    protected:
    const std::regex GetRegex() final { return std::regex(R"([^:]+(.spv))"); }
};

class OutputPathArgument final: public Netero::Console::PositionalArgument<std::string> {
    public:
    const std::string GetName() final { return "<output_path>"; }

    const std::string GetDescription() final { return ".h output file path."; }

    protected:
    const std::regex GetRegex() final { return std::regex(R"([^:]+(.h))"); }
};
