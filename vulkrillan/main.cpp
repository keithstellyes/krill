// added for using debug messaging
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>
#include <iostream>

typedef uint32_t u32;
    VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
u32 pickQueueFamilyIndex(const vk::PhysicalDevice &device)
{
    std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList = device.getQueueFamilyProperties();
    for(u32 i = 0; i < queueFamilyPropertiesList.size(); i++) {
        auto queueFlags = queueFamilyPropertiesList[i].queueFlags;
        // the tutorials and examples seem to suggest just picking the first with gfx support
        if((queueFlags & vk::QueueFlagBits::eGraphics) && (queueFlags & vk::QueueFlagBits::eCompute)){
            return i;
        }
    }

    throw std::runtime_error("Failed to find a graphics queue family!");
}

bool hasSwapchainExtension(vk::PhysicalDevice device) {
    auto extensions = device.enumerateDeviceExtensionProperties();

    for (const auto& ext : extensions) {
        if (std::strcmp(ext.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            return true;
        }
    }
    return false;
}

int deviceScore(const vk::PhysicalDevice &d)
{
    const auto p = d.getProperties();
    int score = 0;
    switch(p.deviceType) {
        case vk::PhysicalDeviceType::eDiscreteGpu:
            score += 1000;
            break;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            // better than a CPU, but hardly.
            score += 10;
            break;
        default:
            break;
    }

    if(hasSwapchainExtension(d)) {
        score += 1001;
    }

    return score;
}

vk::PhysicalDevice pickPhysicalDevice(vk::Instance &instance)
{
    std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

    if(devices.empty()) {
        throw std::runtime_error("No Vulkan devices :(");
    }

    return *std::max_element(devices.begin(), devices.end(), [](const vk::PhysicalDevice a, const vk::PhysicalDevice b) {
            return deviceScore(a) > deviceScore(b);
            });
}

vk::Bool32 VKAPI_CALL debugUtilsMessengerCallback( vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
        vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
        const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
        void * pUserData )
{
    std::cerr << vk::to_string( messageSeverity ) << ": " << vk::to_string( messageTypes );
    std::cerr << " [" << pCallbackData->pMessageIdName << ']';
    std::cerr << pCallbackData->pMessage << ">\n";
    if ( 0 < pCallbackData->queueLabelCount )
    {
        std::cerr << std::string( "\t" ) << "Queue Labels:\n";
        for ( uint32_t i = 0; i < pCallbackData->queueLabelCount; i++ )
        {
            std::cerr << std::string( "\t\t" ) << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
    }
    if ( 0 < pCallbackData->cmdBufLabelCount )
    {
        std::cerr << std::string( "\t" ) << "CommandBuffer Labels:\n";
        for ( uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++ )
        {
            std::cerr << std::string( "\t\t" ) << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }
    // For now, it's a bit noisy to print the objects, but might be useful to do later...
#define PRINT_OBJECTS 0
    if (PRINT_OBJECTS &&  0 < pCallbackData->objectCount )
    {
        std::cerr << std::string( "\t" ) << "Objects:\n";
        for ( uint32_t i = 0; i < pCallbackData->objectCount; i++ )
        {
            std::cerr << std::string( "\t\t" ) << "Object " << i << "\n";
            std::cerr << std::string( "\t\t\t" ) << "objectType   = " << vk::to_string( pCallbackData->pObjects[i].objectType ) << "\n";
            std::cerr << std::string( "\t\t\t" ) << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
            if ( pCallbackData->pObjects[i].pObjectName )
            {
                std::cerr << std::string( "\t\t\t" ) << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
            }
        }
    }
    return vk::False;
}

enum class LogLevel {
    Error,
    Warning,
    Info,
    Verbose
};

vk::DebugUtilsMessengerCreateInfoEXT makeDebugMessenger(const LogLevel &logLevel)
{
    vk::Flags<vk::DebugUtilsMessageSeverityFlagBitsEXT> severityFlags = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    switch(logLevel) {
        case LogLevel::Verbose:
            severityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
        case LogLevel::Info:
            severityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo;
        case LogLevel::Warning:
            severityFlags |= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
        default:
            break;
    }
    return { {}, severityFlags,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation , &debugUtilsMessengerCallback};
}

SDL_Window* createWindow()
{
    return SDL_CreateWindow("My App", 800, 800, SDL_WINDOW_VULKAN);
}

int main()
{
    vk::ApplicationInfo appInfo{
        "My App",
            1,
            "VulKrillan",
            1,
            // TODO AI choce this version is this really the best choice?
            VK_API_VERSION_1_3
    };
    static vk::detail::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    vk::InstanceCreateInfo createInfo{};
    const char *extensions[] = {"VK_EXT_debug_utils", "VK_KHR_surface", "VK_KHR_wayland_surface" /* TODO: let's not assume everyone is using Wayland :) */};
    createInfo.setPpEnabledExtensionNames(extensions);
    createInfo.setEnabledExtensionCount(3);
    createInfo.setPApplicationInfo(&appInfo);

    vk::Instance instance = vk::createInstance(createInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
    vk::DebugUtilsMessengerEXT debugUtilsMessenger = instance.createDebugUtilsMessengerEXT(makeDebugMessenger(LogLevel::Verbose));
    auto physicalDevice = pickPhysicalDevice(instance);
    std::cout << "Chosen physical device: ";
    std::cout << physicalDevice.getProperties().deviceName << std::endl;

    u32 queueIndex = pickQueueFamilyIndex(physicalDevice);
    std::vector<vk::QueueFamilyProperties> queueFamilyPropertiesList = physicalDevice.getQueueFamilyProperties();
    std::cout << "------+-----------+------------\n";
    std::cout << "Queue | Graphics? | Queue Count\n";
    std::cout << "Index |           |\n";
    std::cout << "------+-----------+------------\n";
    for(unsigned int i = 0; i < queueFamilyPropertiesList.size(); i++) {
        const auto properties = queueFamilyPropertiesList[i];
        if(i == queueIndex) {
            std::cout << "\x1b[7m";
        }
        std::cout << i << "     |";
        std::cout << ((properties.queueFlags & vk::QueueFlagBits::eGraphics) ? "    yes    |" : "    no     |");
        std::cout << " " << properties.queueCount;
        if(i == queueIndex) {
            std::cout << "\x1b[0m";
        }
        std::cout << '\n';
    }
    int queueCount = 1;
    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), queueIndex, queueCount, &queuePriority);
    vk::Device device = physicalDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo));
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

    // create a CommandPool to allocate a CommandBuffer from
    vk::CommandPool commandPool = device.createCommandPool(vk::CommandPoolCreateInfo( vk::CommandPoolCreateFlags(), queueIndex));

    // allocate a CommandBuffer from the CommandPool
    vk::CommandBuffer commandBuffer =
        device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, 1)).front();

    SDL_Window *window = createWindow();

    if(window == nullptr) {
        std::cerr << "SDL error:" << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL failed to create window!");
        return -1;
    } else {
        std::cout << "SDL has created window!" << std::endl;
    }

    vk::SurfaceKHR surface;
    {
        VkSurfaceKHR _surface;
        if(!SDL_Vulkan_CreateSurface(window, instance, nullptr, &_surface)) {
            std::cerr << "SDL error:" << SDL_GetError() << std::endl;
            throw std::runtime_error("SDL failed to create Vulkan surface!");
        }
        surface = vk::SurfaceKHR( _surface );
    }

    vk::SurfaceFormatKHR chosenFormat;
    {
        std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(surface);
        std::optional<vk::SurfaceFormatKHR> chosenFormatOptional;
        for(auto &format : formats) {
            std::cout << "Supported format: " << vk::to_string(format.format);
            std::cout << " Color space:" << vk::to_string(format.colorSpace);
            // the format the guide uses. I think it might be a bit basic ?
            if(format.format == vk::Format::eB8G8R8A8Unorm) {
                std::cout << " (good candidate?)";
                // I'm not sure if this is the color space we should do, but it seems like it's good enough
                if(format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                    std::cout << " let's do this one\n";
                    chosenFormatOptional = format;
                    break;
                }
            }
            std::cout << '\n';
        }
        if(!chosenFormatOptional.has_value()) {
            throw std::runtime_error("Failed to find chosen format.");
        }
        chosenFormat = chosenFormatOptional.value();
    }
    std::cout << "SurfaceFormat chosen... Pixel Format: " << vk::to_string(chosenFormat.format) << ", Color space: " << vk::to_string(chosenFormat.colorSpace) << '\n';
    std::cout << "Endering app loop..." << std::endl;
    bool done = false;
    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
    }

    return 0;
}
