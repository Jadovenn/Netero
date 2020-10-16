/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <atomic>
#include <mutex>

#include <Netero/Os.hpp>

#include <CoreFoundation/CoreFoundation.h>
#include <mach-o/dyld.h>
#include <pwd.h>
#include <unistd.h>

namespace Netero::Os {

std::string GetBundlePath()
{
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef    resourcesURL = CFBundleCopyBundleURL(mainBundle);
    CFStringRef cfStr = CFURLCopyFileSystemPath(resourcesURL, kCFURLPOSIXPathStyle);
    CFRelease(resourcesURL);
    char bundlePathStr[PATH_MAX];
    CFStringGetCString(cfStr, bundlePathStr, FILENAME_MAX, kCFStringEncodingASCII);
    CFRelease(cfStr);
    return std::string(bundlePathStr) + "/Contents";
}

std::string GetExecutablePath()
{
    char     path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::string(path);
    }
    return "";
}

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
        return std::string(userInfo->pw_dir) + "/Library";
    }
    return std::string();
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
