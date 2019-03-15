/**
 * Netero sources under BSD-3-Clause
 * see LICENCE.txt
 */

#pragma once

namespace netero {
    /**
     * @brief use for default size allocation in container
     */
    constexpr int page_size = 18;
}

#ifdef WIN32
#define N_WIN32
#endif // WIN32

#ifdef LINUX
#define N_LINUX
#endif // LINUX

#ifdef MACOS
#define N_MACOS
#endif // MACOS