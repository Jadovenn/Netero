/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Console/Arguments.hpp>
#include <Netero/Console/ArgumentsParser.hpp>
#include <Netero/Logger.hpp>

#include "ArgumentSamples.hpp"

#include <gtest/gtest.h>

TEST(NeteroConsole, ProgramArgument_basic_usage)
{
    const char* args[] = { "test_console_usage", "blue", "foo@bar.com", nullptr };
    Netero::Console::ArgumentsParser programArguments;

    programArguments.AddArgument<ColorArgument>();
    programArguments.AddArgument<EmailAddressArgument>();

    programArguments.Execute(3, args);
    EXPECT_EQ(programArguments.GetUnexpectedArgumentsCount(), 0);
    EXPECT_EQ(programArguments.GetMissingArgumentsCount(), 0);
    EXPECT_EQ(programArguments.GetProgramName(), args[0]);
    auto color = programArguments.Get<ColorArgument>();
    EXPECT_EQ(color, Color::BLUE);
    auto email = programArguments.Get<EmailAddressArgument>();
    EXPECT_TRUE(email == std::string(args[2]));
}

TEST(NeteroConsole, ProgramArgument_missing_arg)
{
    const char*                      args[] = { "test_console_usage" };
    Netero::Console::ArgumentsParser programArgument;
    programArgument.AddArgument<ColorArgument>();
    programArgument.AddArgument<EmailAddressArgument>();
    programArgument.Execute(1, args);
    EXPECT_GT(programArgument.GetMissingArgumentsCount(), 1);
}

TEST(NeteroConsole, ProgramArgument_unxepected_arg)
{
    const char*                      args[] = { "test_console_usage", "fo", "foobar", nullptr };
    Netero::Console::ArgumentsParser programArguments;

    programArguments.Execute(3, args);
    EXPECT_EQ(programArguments.GetUnexpectedArgumentsCount(), 2);
}

TEST(NeteroConsole, ProgramArgument_compilation)
{
    Netero::Console::ArgumentsParser programArguments;
    const char* args[] = { "ConsoleArguments", "-v", "Paul", "42", "8.8.8.8:80", nullptr };

    programArguments.AddArgument<VersionOption>();
    programArguments.AddArgument<NameArgument>();
    programArguments.AddArgument<AgeArgument>();
    programArguments.AddArgument<TargetHostArgument>();
    programArguments.SetDescription("A short description.");
    programArguments.Execute(5, args);
    programArguments.ShowUsage();
    if (programArguments.IsPresent<VersionOption>()) {
        LOG << "This is version 1.0" << std::endl;
    }
    std::string name = programArguments.Get<NameArgument>();
    int         age = programArguments.Get<AgeArgument>();
    TargetHost  target = programArguments.Get<TargetHostArgument>();
    LOG << name << " is " << age << " years old." << std::endl;
    LOG << "The target host is " << target.host << " on port " << target.port;
}

TEST(NeteroConsole, ProgramArgument_test_wrong_option)
{
    const char*                      args[] = { "test_console_usage", "-fo", "foobar", nullptr };
    Netero::Console::ArgumentsParser programArguments;
    programArguments.AddArgument<VersionOption>();
    programArguments.AddArgument<ColorArgument>();

    programArguments.Execute(3, args);
    EXPECT_EQ(programArguments.GetUnexpectedArgumentsCount(), 2);
}
