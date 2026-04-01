#include <vulkan/vulkan.hpp>
#include <iostream>

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

    std::vector<vk::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

    if(devices.empty()) {
        throw std::runtime_error("No Vulkan devices :(");
    }
    for (const auto& device : devices) {
        vk::PhysicalDeviceProperties props = device.getProperties();

        std::cout << "Device: " << props.deviceName << "\n";
        std::cout << "Suitability score:" << deviceScore(device) << '\n';

        switch (props.deviceType) {
            case vk::PhysicalDeviceType::eDiscreteGpu:
                std::cout << "Type: Discrete GPU\n";
                break;
            case vk::PhysicalDeviceType::eIntegratedGpu:
                std::cout << "Type: Integrated GPU\n";
                break;
            case vk::PhysicalDeviceType::eCpu:
                std::cout << "Type: CPU\n";
                break;
            default:
                std::cout << "Type: Other\n";
                break;
        }
    }
}
