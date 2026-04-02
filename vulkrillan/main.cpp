#include <vulkan/vulkan.hpp>
#include <iostream>

typedef uint32_t u32;

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

    vk::InstanceCreateInfo createInfo{};
    createInfo.setPApplicationInfo(&appInfo);

    vk::Instance instance = vk::createInstance(createInfo);
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

    return 0;
}
