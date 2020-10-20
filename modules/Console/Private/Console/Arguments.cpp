/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Netero/Console/Arguments.hpp>

namespace Netero::Console {

template<>
bool PositionalArgument<std::string>::AssignMatchResult(std::cmatch& aMatchResult)
{
    myArgumentValue = aMatchResult[0];
    return true;
}

template<>
bool PositionalArgument<int>::AssignMatchResult(std::cmatch& aMatchResult)
{
    myArgumentValue = std::stoi(aMatchResult[0]);
    return true;
}

} // namespace Netero::Console
