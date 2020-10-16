/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#pragma once

/**
 * @file os.hpp
 * @brief Operating System resources lock and standard path getters.
 */

#include <string>

/**
 * Namespace related to os specific resources.
 */
namespace Netero::Os {

/**
 * @brief Get the logged user's username.
 * @return A string containing the username of logged user.
 */
std::string GetSessionUsername();

/**
 * @brief Get the user's home directory path.
 */
std::string GetUserHomeDirectoryPath();

/**
 * @brief Get the user's app data path.
 * This is the directory you are likely to create a folder
 * with the name of your application and store config files.
 */
std::string GetUserAppDataRoamingPath();

/**
 * @brief Return the path to the application bundle if there is.
 * On linux this may return ".".
 */
std::string GetBundlePath();

/**
 * @brief Return path to the current executable
 */
std::string GetExecutablePath();

/**
 * @brief Perform necessary init call if needed
 * This help you while you are using netero beside
 * other libraries that may require access to the COM
 * interface on windows for example.
 * For example on windows Netero will try to init the COM
 * library, if it can not it will simply conform to the current
 * initialization. This is used by the audio submodule.
 */
void AcquireSystemResources();

/**
 * @brief Release the OS handle if necessary
 */
void ReleaseSystemResources();

/**
 * @brief Return the number of locks, might be non zero, netero submodule
 * may use this function set to access COM interfaces for example.
 */
int GetSystemResourcesLocks();

/**
 * @brief Tell you if Netero have preform initialization.
 */
bool IsSystemLibraryHolder();

} // namespace Netero::Os
