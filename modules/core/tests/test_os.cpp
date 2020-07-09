/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <cassert>
#include <iostream>

#include <netero/os.hpp>

int main()
{
    netero::os::acquireSystemResources();
    assert(netero::os::getSystemResourcesLocks() == 1);
    netero::os::releaseSystemResources();
    assert(netero::os::getSystemResourcesLocks() == 0);

    std::cout << netero::os::getSessionUsername() << std::endl;
    std::cout << netero::os::getUserHomeDirectoryPath() << std::endl;
    std::cout << netero::os::getUserAppDataRoamingPath() << std::endl;
    std::cout << netero::os::getBundlePath() << std::endl;
    std::cout << netero::os::getExecutablePath() << std::endl;

    return 0;
}
