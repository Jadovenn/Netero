/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <mach-o/dyld.h>
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <pwd.h>
#include <mutex>
#include <atomic>
#include <netero/os.hpp>

std::string	netero::os::getBundlePath() {
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
	CFStringRef cfStr = CFURLCopyFileSystemPath(resourcesURL, kCFURLPOSIXPathStyle);
	CFRelease(resourcesURL);
	char bundlePathStr[PATH_MAX];
	CFStringGetCString(cfStr, bundlePathStr, FILENAME_MAX, kCFStringEncodingASCII);
	CFRelease(cfStr);
	return std::string(bundlePathStr) + "/Contents";
}

std::string netero::os::getExecutablePath() {
	char path[1024];
	uint32_t size = sizeof(path);
	if (_NSGetExecutablePath(path, &size) == 0) {
		return std::string(path);
	}
	return "";
}

std::string netero::os::getSessionUsername() {
	uid_t uid = getuid();
	struct passwd *userInfo = getpwuid(uid);
	if (userInfo) {
		return std::string(userInfo->pw_name);
	}
	return std::string();
}

std::string netero::os::getUserHomeDirectoryPath() {
	uid_t uid = getuid();
	struct passwd *userInfo = getpwuid(uid);
	if (userInfo) {
		return std::string(userInfo->pw_dir);
	}
	return std::string();
}

std::string netero::os::getUserAppDataRoamingPath() {
	uid_t uid = getuid();
	struct passwd *userInfo = getpwuid(uid);
	if (userInfo) {
		return std::string(userInfo->pw_dir) + "/Library";
	}
	return std::string();
}

static std::atomic<int>     g_com_library_locks = 0;
static std::mutex           g_com_lock_mutex;

void netero::os::acquireSystemResources() {
	g_com_library_locks += 1;
}

void netero::os::releaseSystemResources() {
	if (g_com_library_locks.load(std::memory_order_acquire) > 0) {
		g_com_library_locks -= 1;
	}
}

int netero::os::getSystemResourcesLocks() {
	return g_com_library_locks.load(std::memory_order_acquire);
}

bool netero::os::isSystemLibraryHolder() {
	return false;
}