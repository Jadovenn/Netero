/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <atomic>
#include <mutex>

#include <Netero/Netero.hpp>
#include <Netero/Os.hpp>

#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

namespace Netero::Os {

std::string GetSessionUsername()
{
    uid_t          uid = getuid();
    struct passwd *userInfo = getpwuid(uid);
    if (userInfo) {
        return std::string(userInfo->pw_name);
    }
    return std::string();
}

std::string GetUserHomeDirectoryPath()
{
    uid_t          uid = getuid();
    struct passwd *userInfo = getpwuid(uid);
    if (userInfo) {
        return std::string(userInfo->pw_dir);
    }
    return std::string();
}

std::string GetUserAppDataRoamingPath()
{
    uid_t          uid = getuid();
    struct passwd *userInfo = getpwuid(uid);
    if (userInfo) {
        return std::string(userInfo->pw_dir);
    }
    return std::string();
}

std::string GetBundlePath()
{
    return netero::isDebugMode ? "." : "/usr/share";
}

std::string GetExecutablePath()
{
    char path[PATH_MAX];
    char dest[PATH_MAX];
    memset(dest, 0, sizeof(dest));
    pid_t pid = getpid();
    snprintf(path, "/proc/%ld/exe", static_cast<long>(pid));
    ssize_t result = readlink(path, dest, PATH_MAX);
    if (result == -1 || static_cast<size_t>(result) >= sizeof(dest)) {
        return "";
    }
    return std::string(path);
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
