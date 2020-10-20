/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Console/Arguments.hpp>
#include <Netero/Console/ArgumentsParser.hpp>
#include <Netero/Logger.hpp>

#include "ArgumentSamples.hpp"

#include <gtest/gtest.h>

TEST(NeteroConsole, ArgumentParser_usage_without_option_test)
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

TEST(NeteroConsole, ArgumentParser_missing_argument_test)
{
    const char*                      args[] = { "test_console_usage" };
    Netero::Console::ArgumentsParser programArgument;
    programArgument.AddArgument<ColorArgument>();
    programArgument.AddArgument<EmailAddressArgument>();
    EXPECT_FALSE(programArgument.AddArgument<EmailAddressArgument>());
    programArgument.Execute(1, args);
    EXPECT_GT(programArgument.GetMissingArgumentsCount(), 1);
}

TEST(NeteroConsole, ArgumentParser_no_argument_test)
{
    const char*                      args[] = { "test_console_usage", "fo", "foobar", nullptr };
    Netero::Console::ArgumentsParser programArguments;

    programArguments.Execute(3, args);
    EXPECT_EQ(programArguments.GetUnexpectedArgumentsCount(), 2);
}

TEST(NeteroConsole, ArgumentParser_correct_usage_test)
{
    Netero::Console::ArgumentsParser programArguments;
    const char* args[] = { "ConsoleArguments", "-v", "Paul", "42", "8.8.8.8:80", nullptr };

    programArguments.AddArgument<VersionOption>();
    programArguments.AddArgument<NameArgument>();
    programArguments.AddArgument<AgeArgument>();
    programArguments.AddArgument<TargetHostArgument>();
    programArguments.SetDescription("A short description.");
    programArguments.Execute(5, args);
    if (programArguments.IsPresent<VersionOption>()) {
        LOG << "This is version 1.0" << std::endl;
    }
    std::string name = programArguments.Get<NameArgument>();
    int         age = programArguments.Get<AgeArgument>();
    TargetHost  target = programArguments.Get<TargetHostArgument>();
    LOG << name << " is " << age << " years old." << std::endl;
    LOG << "The target host is " << target.host << " on port " << target.port;
}

TEST(NeteroConsole, ArgumentParser_wrong_option_test)
{
    const char* args[] = { "test_console_usage", "-fo", "red", "foobar", nullptr };
    Netero::Console::ArgumentsParser programArguments;
    programArguments.AddArgument<VersionOption>();
    programArguments.AddArgument<ColorArgument>();

    programArguments.Execute(4, args);
    EXPECT_EQ(programArguments.GetUnexpectedArgumentsCount(), 2);
}

TEST(NeteroConsole, ArgumentParser_wrong_argument_test)
{
    const char* args[] = { "test_console_usage", "-fo", "yellow", "foobar", nullptr };
    Netero::Console::ArgumentsParser programArguments;
    programArguments.AddArgument<VersionOption>();
    programArguments.AddArgument<ColorArgument>();

    programArguments.Execute(4, args);
    EXPECT_EQ(programArguments.GetUnexpectedArgumentsCount(), 3);
    EXPECT_EQ(programArguments.GetMissingArgumentsCount(), 0);
}

TEST(NeteroConsole, ArgumentParser_usage_test)
{
    const char* args[] = { "test_console_usage", "-v",         "hello", "12",
                           "foo@bar.com",        "8.8.8.8:80", "red",   nullptr };

    Netero::Console::ArgumentsParser argumentsParser;
    argumentsParser.AddArgument<VersionOption>();
    argumentsParser.AddArgument<NameArgument>();
    argumentsParser.AddArgument<AgeArgument>();
    argumentsParser.AddArgument<EmailAddressArgument>();
    argumentsParser.AddArgument<TargetHostArgument>();
    argumentsParser.AddArgument<ColorArgument>();
    argumentsParser.Execute(7, args);
    argumentsParser.ShowUsage();
}

TEST(NeteroConsole, ArgumentParser_assignMatchResult_error_test)
{
    const char* args[] = { "test_console_usage", "8.8.8.8:70000", nullptr };

    Netero::Console::ArgumentsParser argumentsParser;
    argumentsParser.AddArgument<TargetHostArgument>();
    argumentsParser.Execute(2, args);
    EXPECT_EQ(argumentsParser.GetUnexpectedArgumentsCount(), 1);
}
