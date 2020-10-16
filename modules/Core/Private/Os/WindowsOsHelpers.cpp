/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

// clang-format off

#include <windows.h>

#include <Lmcons.h>
#include <UserEnv.h>
#include <combaseapi.h>
#include <shlobj_core.h>

#include <atomic>
#include <mutex>

#include <Netero/Os.hpp>

// clang-format off

namespace Netero::Os {


std::string Netero::Os::GetSessionUsername()
{
    char  username[UNLEN + 1];
    DWORD len = UNLEN + 1;

    if (GetUserName(username, &len)) {
        return std::string(username);
    }
    return std::string();
}

std::string GetUserHomeDirectoryPath()
{
    char  path[32];
    DWORD len = 32;

    if (GetProfilesDirectoryA(path, &len)) {
        return std::string(path) + "\\" + netero::os::getSessionUsername();
    }
    return std::string();
}

std::string GetUserAppDataRoamingPath()
{
    PWSTR str;

    const HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &str);
    if (FAILED(result)) {
        return std::string();
    }
    std::string string;
    size_t      converted_char = 0;
    size_t      size = wcslen(str);
    size *= 2;

    if (!str) {
        return string;
    }
    const auto c_string = new (std::nothrow) char[size];
    if (!c_string) {
        return string;
    }
    wcstombs_s(&converted_char, c_string, size, str, _TRUNCATE);
    string = c_string;
    delete[] c_string;
    return string;
}

std::string GetBundlePath()
{
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::string string;
    size_t      converted_char = 0;
    size_t      size = wcslen(path);
    size *= 2;

    if (!path) {
        return string;
    }
    const auto c_string = new (std::nothrow) char[size];
    if (!c_string) {
        return string;
    }
    wcstombs_s(&converted_char, c_string, size, path, _TRUNCATE);
    string = c_string;
    delete[] c_string;
    const auto pos = string.find_last_of('\\');
    if (pos != std::string::npos) {
        string.erase(pos);
    }
    return string;
}

std::string GetExecutablePath()
{
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::string string;
    size_t      converted_char = 0;
    size_t      size = wcslen(path);
    size *= 2;

    if (!path) {
        return string;
    }
    const auto c_string = new (std::nothrow) char[size];
    if (!c_string) {
        return string;
    }
    wcstombs_s(&converted_char, c_string, size, path, _TRUNCATE);
    string = c_string;
    delete[] c_string;
    return string;
}

static std::atomic<int>  g_com_library_locks = 0;
static std::mutex        g_com_lock_mutex;
static std::atomic<bool> g_is_com_holder = false;

void AcquireSystemResources()
{
    if (g_com_library_locks.load(std::memory_order_acquire) == 0) {
        std::scoped_lock<std::mutex> lock(g_com_lock_mutex);
        const HRESULT                result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (result == S_OK) {
            g_is_com_holder = true;
        }
        g_com_library_locks += 1;
    }
    else {
        g_com_library_locks += 1;
    }
}

void ReleaseSystemResources()
{
    if (g_com_library_locks.load(std::memory_order_acquire) > 0) {
        g_com_library_locks -= 1;
        if (g_is_com_holder.load(std::memory_order_acquire) == true &&
            g_com_library_locks.load(std::memory_order_acquire) == 0) {
            std::scoped_lock<std::mutex> lock(g_com_lock_mutex);
            CoUninitialize();
            g_is_com_holder = false;
        }
    }
}

int GetSystemResourcesLocks()
{
    return g_com_library_locks.load(std::memory_order_acquire);
}

bool IsSystemLibraryHolder()
{
    return g_is_com_holder.load(std::memory_order_acquire);
}

} // namespace Netero::Os
