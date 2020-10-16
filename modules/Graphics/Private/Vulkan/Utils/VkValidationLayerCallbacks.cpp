/**
 * Netero sources under BSD-3-Clause
 * see LICENSE.txt
 */

#include <Vulkan/VkUtils.hpp>

#include <Netero/Logger.hpp>

namespace VkUtils {

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugMessengerCallback;
    createInfo.pUserData = nullptr; // Optional
    createInfo.pNext = nullptr;
}

void PopulateDebugReportCreateInfo(VkDebugReportCallbackCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    createInfo.pfnCallback = DebugReportCallback;
    createInfo.pUserData = nullptr;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
DebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void*)
{
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        LOG_INFO << pCallbackData->pMessage << std::endl;
    }
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_WARNING << pCallbackData->pMessage << std::endl;
    }
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOG_ERROR << pCallbackData->pMessage << std::endl;
    }
    return VK_TRUE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkFlags                    msgFlags,
                                                   VkDebugReportObjectTypeEXT objType,
                                                   uint64_t                   srcObject,
                                                   size_t                     location,
                                                   int32_t                    msgCode,
                                                   const char*                pLayerPrefix,
                                                   const char*                pMsg,
                                                   void*                      pUserData)
{
    return VK_FALSE;
}

} // namespace VkUtils
