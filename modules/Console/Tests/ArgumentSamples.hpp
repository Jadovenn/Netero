/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <Netero/Console/Arguments.hpp>

/******************************************
 * Example for option
*******************************************/

class VersionOption final: public Netero::Console::Option {
    public:
    const std::string GetName() final { return "-v, --version"; }

    const std::string GetDescription() final { return "Display the program version."; }

    protected:
    const std::pair<std::string, std::string> GetOptionVariant() final
    {
        return { "-v", "--version" };
    }
};

/******************************************
 * Example with scalar and standard types
*******************************************/

class NameArgument final: public Netero::Console::PositionalArgument<std::string> {
    public:
    const std::string GetName() final { return "<NAME>"; }

    const std::string GetDescription() final { return "The person name."; }

    protected:
    const std::regex GetRegex() final { return std::regex(R"([\w'-]+)"); }
};

class AgeArgument final: public Netero::Console::PositionalArgument<int> {
    public:
    const std::string GetName() final { return "<AGE>"; }

    const std::string GetDescription() final { return "The person age."; }

    protected:
    const std::regex GetRegex() { return std::regex(R"(\d+)"); }
};

class EmailAddressArgument final: public Netero::Console::PositionalArgument<std::string> {
    public:
    const std::string GetName() final { return "<EMAIL>"; }

    const std::string GetDescription() final { return "The person e-mail address."; }

    protected:
    const std::regex GetRegex() { return std::regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)"); }
};

/******************************************
 * Example with a custom type
*******************************************/

struct TargetHost {
    std::string host;
    int         port;
};

class TargetHostArgument final: public Netero::Console::PositionalArgument<TargetHost> {
    public:
    const std::string GetName() final { return "<TARGET_HOST>:<PORT>"; }

    const std::string GetDescription() final { return "The target host."; }

    protected:
    const std::regex GetRegex() { return std::regex(R"(^([^:]+):([0-9]+)$)"); }
};

namespace Netero::Console {

template<>
bool PositionalArgument<TargetHost>::AssignMatchResult(std::cmatch &aMatchResult)
{
    myArgumentValue.host = aMatchResult[1];
    myArgumentValue.port = std::stoi(aMatchResult[2]);
    return true;
}

} // namespace Netero::Console

enum Color { RED = 1, BLUE = 2 };

class ColorArgument final: public Netero::Console::PositionalArgument<Color> {
    public:
    const std::string GetName() final { return "<COLOR>"; }

    const std::string GetDescription() final { return "A color either blue or red."; }

    protected:
    const std::regex GetRegex() { return std::regex(R"(^(blue|red)$)"); }
};

namespace Netero::Console {

template<>
bool PositionalArgument<Color>::AssignMatchResult(std::cmatch &aMatchResult)
{
    if (aMatchResult[0] == "red") {
        myArgumentValue = Color::RED;
    }
    else {
        myArgumentValue = Color::BLUE;
    }
    return true;
}

} // namespace Netero::Console
