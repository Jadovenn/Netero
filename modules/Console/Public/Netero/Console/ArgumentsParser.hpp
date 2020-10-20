/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Netero/Console/Arguments.hpp>
#include <Netero/TypeId.hpp>

namespace Netero::Console {

class ArgumentsParser {
    public:
    ArgumentsParser() = default;
    ~ArgumentsParser()
    {
        myPositionalArguments.clear();
        myOptions.clear();
        myMissingArguments.clear();
        myUnexpectedArguments.clear();
        myArguments.clear();
    }

    void Execute(int argc, const char** argv);

    template<class Arg>
    bool IsPresent()
    {
        auto it = myArguments.find(ArgumentTypeId::GetTypeID<Arg>());
        if (it == myArguments.end()) {
            assert("Arg template type must be an added parameter.");
        }
        Arg* argument = dynamic_cast<Arg*>(it->second.get());
        return argument->IsPresent();
    }

    template<class Arg>
    typename Arg::ArgumentType Get()
    {
        auto it = myArguments.find(ArgumentTypeId::GetTypeID<Arg>());
        if (it == myArguments.end()) {
            assert("Arg template type must be an added parameter.");
        }
        Arg* argument = dynamic_cast<Arg*>(it->second.get());
        return argument->Get();
    }

    template<typename Arg,
             typename... CtorArgs,
             typename = std::enable_if<std::is_base_of<Argument, Arg>::value>>
    void AddArgument(CtorArgs... someArguments)
    {
        auto argument = std::make_unique<Arg>(someArguments...);
        if (std::is_base_of<Option, Arg>::value) {
            myOptions.emplace_back(dynamic_cast<Argument*>(argument.get()));
        }
        else {
            myPositionalArguments.emplace_back(dynamic_cast<Argument*>(argument.get()));
        }
        myArguments.emplace(ArgumentTypeId::GetTypeID<Arg>(),
                            dynamic_cast<Argument*>(argument.release()));
    }

    [[nodiscard]] const std::vector<std::string>& GetMissingArguments() const
    {
        return myMissingArguments;
    }
    [[nodiscard]] size_t GetMissingArgumentsCount() const { return myMissingArguments.size(); }

    [[nodiscard]] const std::vector<std::pair<std::string, std::string>>&
    GetUnexpectedArguments() const
    {
        return myUnexpectedArguments;
    }
    [[nodiscard]] size_t GetUnexpectedArgumentsCount() const
    {
        return myUnexpectedArguments.size();
    }

    [[nodiscard]] const char* GetProgramName() const { return myProgramName; }

    void SetFooter(const std::string& aFooter) { myFooter = aFooter; }

    void SetDescription(const std::string& aDescription) { myDescription = aDescription; }

    void ShowUsage();

    private:
    using ArgumentTypeId = Netero::TypeID<Argument>;

    const char*                                      myProgramName;
    std::string                                      myFooter;
    std::string                                      myDescription;
    std::vector<std::string>                         myMissingArguments;
    std::vector<std::pair<std::string, std::string>> myUnexpectedArguments;
    int                                              myCount;

    std::vector<Argument*>                                         myPositionalArguments;
    std::vector<Argument*>                                         myOptions;
    std::unordered_map<Netero::type_id, std::unique_ptr<Argument>> myArguments;
};

} // namespace Netero::Console
