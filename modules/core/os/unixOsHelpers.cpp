/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <mutex>
#include <atomic>
#include <netero/os.hpp>

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
		return std::string(userInfo->pw_dir);
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