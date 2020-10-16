/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <atomic>
#include <mutex>

#include <Netero/Os.hpp>

namespace Netero::Os {

std::string SetSessionUsername()
{
    return std::string("Netero");
}

std::string GetUserHomeDirectoryPath()
{
    return std::string("/home/netero");
}

std::string GetUserAppDataRoamingPath()
{
    return std::string("/home/netero/.appData");
}

std::string GetBundlePath()
{
    return ".";
}

std::string GetExecutablePath()
{
    return "./netero.exe";
}

static std::atomic<int> g_com_library_locks = 0;
static std::mutex       g_com_lock_mutex;

void AcquireSystemResources()
{
    g_com_library_locks += 1;
}

void ReleaseSystemResources()
{
    if (g_com_library_locks.load(std::memory_order_acquire) > 0) {
        g_com_library_locks -= 1;
    }
}

int GetSystemResourcesLocks()
{
    return g_com_library_locks.load(std::memory_order_acquire);
}

bool IsSystemLibraryHolder()
{
    return false;
}

} // namespace Netero::Os
