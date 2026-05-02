// added for using debug messaging
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#include <fstream>
#include <iostream>

typedef uint32_t u32;
    VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
u32 pickGraphicsQueueFamilyIndex(const vk::PhysicalDevice &device)
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

vk::Extent2D pickExtent(const vk::SurfaceCapabilitiesKHR &surfaceCapabilities, uint32_t width, uint32_t height)
{
    vk::Extent2D               swapchainExtent;
    if ( surfaceCapabilities.currentExtent.width == (std::numeric_limits<uint32_t>::max)() )
    {
        // If the surface size is undefined, the size is set to the size of the images requested.
        swapchainExtent.width  = std::clamp(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width );
        swapchainExtent.height = std::clamp(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height );
    }
    else
    {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfaceCapabilities.currentExtent;
    }

    return swapchainExtent;
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
    std::cout << "Swapchain extension name:" << VK_KHR_SWAPCHAIN_EXTENSION_NAME << std::endl;

    vk::InstanceCreateInfo createInfo{};
    std::vector<const char*> extensions = {"VK_EXT_debug_utils",
        "VK_KHR_surface",
        "VK_KHR_wayland_surface" /* TODO: let's not assume everyone is using Wayland :) */};
    createInfo.setPpEnabledExtensionNames(extensions.data());
    createInfo.setEnabledExtensionCount(extensions.size());
    createInfo.setPApplicationInfo(&appInfo);
    std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.setPpEnabledLayerNames(validationLayers.data());
    createInfo.setEnabledLayerCount(validationLayers.size());

    vk::Instance instance = vk::createInstance(createInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
    vk::DebugUtilsMessengerEXT debugUtilsMessenger = instance.createDebugUtilsMessengerEXT(makeDebugMessenger(LogLevel::Verbose));
    auto physicalDevice = pickPhysicalDevice(instance);
    std::cout << "Chosen physical device: ";
    std::cout << physicalDevice.getProperties().deviceName << std::endl;

    u32 queueIndex = pickGraphicsQueueFamilyIndex(physicalDevice);
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
    std::vector<const char*> requiredDeviceExtensions = {vk::KHRSwapchainExtensionName, "VK_KHR_shader_draw_parameters"};
    vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo);
    deviceCreateInfo.setPpEnabledExtensionNames(requiredDeviceExtensions.data());
    deviceCreateInfo.setEnabledExtensionCount(requiredDeviceExtensions.size());
    vk::Device device = physicalDevice.createDevice(deviceCreateInfo);
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
    uint32_t presentQueueIndex = std::numeric_limits<uint32_t>::max();
    for(unsigned int i = 0; i < queueFamilyPropertiesList.size(); i++) {
        if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
            std::cout << "Present queue:" << i << '\n';
            presentQueueIndex = i;
        }
    }
    if(presentQueueIndex == std::numeric_limits<uint32_t>::max()) {
        throw std::runtime_error("present queue not found!");
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
                // I'm not sure if this is the color space we should do, but it seems like it's good enough and is used in examples
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
    const auto &surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    vk::Extent2D extent = pickExtent(surfaceCapabilities, 800, 800);
    vk::SwapchainCreateInfoKHR swapChainCreateInfo( vk::SwapchainCreateFlagsKHR(),
            surface,
            std::clamp(3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount),
            chosenFormat.format,
            chosenFormat.colorSpace,
            extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            {},
            surfaceCapabilities.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            vk::PresentModeKHR::eFifo,
            true,
            nullptr);
    std::cout << "Swapchain created!\n";
    vk::SwapchainKHR swapChain = device.createSwapchainKHR(swapChainCreateInfo);
    std::vector<vk::Image> swapChainImages = device.getSwapchainImagesKHR(swapChain);
    std::cout << swapChainImages.size() << " images in swap chain!\n";

    vk::AttachmentDescription colorAttachment(
            {},                                  // flags
            chosenFormat.format,                     // MUST match swapchain
            vk::SampleCountFlagBits::e1,
            vk::AttachmentLoadOp::eClear,        // clear at start
            vk::AttachmentStoreOp::eStore,       // keep result for presentation
            vk::AttachmentLoadOp::eDontCare,     // stencil
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,         // layout before render pass
            vk::ImageLayout::ePresentSrcKHR      // layout after render pass
            );
    vk::AttachmentReference colorAttachmentRef(
            0, // index into attachment list
            vk::ImageLayout::eColorAttachmentOptimal
            );
    vk::SubpassDescription subpass(
            {},
            vk::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, &colorAttachmentRef
            );
    vk::SubpassDependency dependency(
            VK_SUBPASS_EXTERNAL,
            0,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
            {},
            vk::AccessFlagBits::eColorAttachmentWrite
            );
    vk::RenderPassCreateInfo renderPassInfo(
            {},
            1, &colorAttachment,
            1, &subpass,
            1, &dependency
            );

    vk::RenderPass renderPass = device.createRenderPass(renderPassInfo);
    std::vector<vk::ImageView> imageViews;
    imageViews.reserve( swapChainImages.size() );
    vk::ImageViewCreateInfo imageViewCreateInfo( {}, {}, vk::ImageViewType::e2D, chosenFormat.format, {}, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } );
    for ( auto image : swapChainImages )
    {
        imageViewCreateInfo.image = image;
        imageViews.push_back( device.createImageView( imageViewCreateInfo ) );
    }
    std::vector<vk::Framebuffer> framebuffers;
    for (auto view : imageViews) {
        vk::FramebufferCreateInfo fbInfo(
                {},
                renderPass,
                1, &view,
                extent.width,
                extent.height,
                1
                );

        framebuffers.push_back(device.createFramebuffer(fbInfo));
    }

    bool done = false;

    std::ifstream shaderFile("shader.spv", std::ios::ate | std::ios::binary);

    if (!shaderFile.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t byteLen = shaderFile.tellg();
    uint32_t *spirvBuffer = static_cast<uint32_t*>(malloc(byteLen));
    if(spirvBuffer == nullptr) {
        throw std::runtime_error("Failed to alloc buffer for spirv data");
    }
    shaderFile.seekg(0, std::ios::beg);
    shaderFile.read((char*)spirvBuffer, byteLen);
    shaderFile.close();
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), byteLen, spirvBuffer);
    vk::ShaderModule vertexShaderModule = device.createShaderModule(shaderModuleCreateInfo);
    vk::ShaderModule fragShaderModule = device.createShaderModule( shaderModuleCreateInfo );
    vk::PipelineShaderStageCreateInfo vertexStage( vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "vertMain");
    vk::PipelineShaderStageCreateInfo fragStage( vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, fragShaderModule, "fragMain");
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo(
            {}, 0, nullptr, 0, nullptr
            );
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
            {}, vk::PrimitiveTopology::eTriangleList, false
            );
    vk::Viewport viewport(0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f);
    vk::Rect2D scissor({0, 0}, {extent.width, extent.height});

    vk::PipelineViewportStateCreateInfo viewportState(
            {}, 1, &viewport, 1, &scissor
            );
    vk::PipelineRasterizationStateCreateInfo rasterizer(
            {},
            VK_FALSE, // depthClampEnable
            VK_FALSE, // rasterizerDiscardEnable
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack,
            vk::FrontFace::eClockwise, // NOTE: depends on your coordinate system
            VK_FALSE,
            0, 0, 0,
            1.0f
            );
    vk::PipelineMultisampleStateCreateInfo multisampling(
            {},
            vk::SampleCountFlagBits::e1,
            VK_FALSE
            );
    vk::PipelineColorBlendAttachmentState colorBlendAttachment(
            VK_FALSE
            );
    colorBlendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colorBlending(
            {}, VK_FALSE, vk::LogicOp::eCopy,
            1, &colorBlendAttachment
            );
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    vk::PipelineLayout pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
    std::array<vk::PipelineShaderStageCreateInfo, 2> stages = {
        vertexStage, fragStage
    };

    vk::GraphicsPipelineCreateInfo pipelineInfo(
            {},
            stages.size(), stages.data(),
            &vertexInputInfo,
            &inputAssembly,
            nullptr,
            &viewportState,
            &rasterizer,
            &multisampling,
            nullptr,
            &colorBlending,
            nullptr,
            pipelineLayout,
            renderPass
            );
    pipelineInfo.renderPass = renderPass;


    vk::Pipeline pipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;
    auto cmd = commandBuffer;

    cmd.begin(vk::CommandBufferBeginInfo());

    vk::ClearValue clearColor(
            std::array<float,4>{0.0f, 0.0f, 0.0f, 1.0f}
            );

    vk::RenderPassBeginInfo rpInfo(
            renderPass,
            // are we supposed to enumerate over framebuffers?
            framebuffers[0],
            {{0,0}, extent},
            1,
            &clearColor
            );

    cmd.beginRenderPass(rpInfo, vk::SubpassContents::eInline);

    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

    cmd.draw(3, 1, 0, 0); // ← your triangle!

    cmd.endRenderPass();
    cmd.end();
    vk::Semaphore imageAvailable = device.createSemaphore({});
    vk::Semaphore renderFinished = device.createSemaphore({});
    vk::Fence inFlight = device.createFence({vk::FenceCreateFlagBits::eSignaled});
    device.waitForFences(inFlight, VK_TRUE, UINT64_MAX);
    device.resetFences(inFlight);

    uint32_t imageIndex = device.acquireNextImageKHR(
            swapChain,
            UINT64_MAX,
            imageAvailable
            ).value;

    vk::PipelineStageFlags waitStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    vk::SubmitInfo submitInfo(
            1, &imageAvailable, &waitStage,
            1, &commandBuffer,
            1, &renderFinished
            );

    // TODO we need logic to determine the graphics queue (related to queue families?)
    // chatgpt provided these
    vk::Queue graphicsQueue = device.getQueue(pickGraphicsQueueFamilyIndex(physicalDevice), 0);
    vk::Queue presentQueue  = device.getQueue(presentQueueIndex, 0);
    graphicsQueue.submit(submitInfo, inFlight);

    vk::PresentInfoKHR presentInfo(
            1, &renderFinished,
            1, &swapChain,
            &imageIndex
            );

    presentQueue.presentKHR(presentInfo);
    std::cout << "main app loop!" << std::endl;
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
