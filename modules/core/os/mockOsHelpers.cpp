/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <mutex>
#include <atomic>
#include <netero/os.hpp>

std::string netero::os::getSessionUsername() {
	return std::string("Netero");
}

std::string netero::os::getUserHomeDirectoryPath() {
	return std::string("/home/netero");
}

std::string netero::os::getUserAppDataRoamingPath() {
	return std::string("/home/netero/.appData");
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