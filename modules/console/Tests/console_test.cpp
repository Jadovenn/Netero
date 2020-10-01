/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/ProgramArguments.hpp>

#include <gtest/gtest.h>

TEST(NeteroConsole, ProgramArgument_basic_usage)
{
    Netero::Console::ProgramArguments programArguments;
    programArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                 "ARG1",
                                 "^(foo|bar)$");
    programArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                 "ARG2",
                                 R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    const char* args[] = { "test_console_usage", "foo", "foo@bar.com", nullptr };
    programArguments.Parse(3, args);
    EXPECT_EQ(programArguments.UnexpectedCount(), 0);
    EXPECT_TRUE(programArguments.MissingArgs().empty());
    EXPECT_EQ(programArguments.ProgramName(), args[0]);
    auto* result = programArguments.GetAsStrings("ARG1");
    EXPECT_TRUE(((*result)[0] == std::string(args[1])));
    result = programArguments.GetAsStrings("ARG2");
    EXPECT_TRUE(((*result)[0] == std::string(args[2])));

    programArguments.clear();
}

TEST(NeteroConsole, ProgramArgument_missing_arg)
{
    Netero::Console::ProgramArguments programArguments;
    programArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                 "ARG1",
                                 "^(foo|bar)$");
    programArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                 "ARG2",
                                 R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    const char* args_missing_args[] = { "test_console_usage" };
    programArguments.Parse(1, args_missing_args);
    EXPECT_GT(programArguments.MissingArgs().size(), 1);
}

TEST(NeteroConsole, ProgramArgument_unxepected_arg)
{
    Netero::Console::ProgramArguments programArguments;
    programArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                 "ARG1",
                                 "^(foo|bar)$");
    programArguments.AddArgument(Netero::Console::ProgramArgumentType::ARGUMENT,
                                 "ARG2",
                                 R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    const char* args[] = { "test_console_usage", "fo", "foobar", nullptr };
    programArguments.Parse(3, args);
    EXPECT_EQ(programArguments.UnexpectedCount(), 2);
}
