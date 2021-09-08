// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VK_USE_PLATFORM_WIN32_KHR 1
#define VKB_VALIDATION_LAYERS
#define VKB_DEBUG 1
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fstream>
#include <vk_mem_alloc.h>
#include "C:/VulkanSDK/1.2.182.0/Include/shaderc/shaderc.hpp"
#include "MeshManager.h"
#include "VertexBuffer.h"
#include "logger.hpp"

//#define LOGGER_FORMAT "[%^%l%$] %v"
//#define ROOT_PATH_SIZE 48
//#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)
//
//#define LOGI(...) spdlog::info(__VA_ARGS__);
//#define LOGW(...) spdlog::warn(__VA_ARGS__);
//#define LOGE(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
//#define LOGD(...) spdlog::debug(__VA_ARGS__);
#pragma region VK check
#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout << "Detected Vulkan error: " << err << std::endl; \
			 											\
			abort();                                                \
		}                                                           \
	} while (0)
#pragma endregion



constexpr uint32_t Width = 1024;
constexpr uint32_t Height = 768;

struct Allocation {
	vk::Device device;
	vk::DeviceMemory memory;
	vk::DeviceSize size{ 0 };
	vk::DeviceSize alignment{ 0 };
	vk::DeviceSize allocSize{ 0 };
	void* mapped{ nullptr };
	/** @brief Memory propertys flags to be filled by external source at buffer creation (to query at some later point) */
	vk::MemoryPropertyFlags memoryPropertyFlags;

};
struct Image : Allocation
{
	vk::Image image;
	vk::Extent3D extent;
	vk::ImageView view;
	vk::Sampler sampler;
	vk::Format format{ vk::Format::eUndefined };
	VmaAllocation allocation;
};
struct SwapchainDimensions
{
	/// Width of the swapchain.
	uint32_t width = 0;

	/// Height of the swapchain.
	uint32_t height = 0;

	/// Pixel format of the swapchain.
	vk::Format format = vk::Format::eUndefined;
};
struct PerFrame
{
	vk::Device device;

	vk::Fence queue_submit_fence;

	vk::CommandPool primary_command_pool;

	vk::CommandBuffer primary_command_buffer;

	vk::Semaphore swapchain_acquire_semaphore;

	vk::Semaphore swapchain_release_semaphore;

	int32_t queue_index;
};
struct VulkanContext
{
	VulkanContext& operator= (const VulkanContext&) = delete;
	//Context(Context& other) = delete;
	/// The Vulkan instance.
	vk::Instance instance;

	/// The Vulkan physical device.
	vk::PhysicalDevice gpu;

	/// The Vulkan device.
	vk::Device device;

	/// The Vulkan device queue.
	vk::Queue queue;

	/// The swapchain.
	vk::SwapchainKHR swapchain;

	/// The swapchain dimensions.
	SwapchainDimensions swapchain_dimensions;

	/// The surface we will render to.
	vk::SurfaceKHR surface;

	/// The queue family index where graphics work will be submitted.
	int32_t graphics_queue_index = -1;

	/// The image view for each swapchain image.
	std::vector<vk::ImageView> swapchain_image_views;

	/// The framebuffer for each swapchain image view.
	std::vector<vk::Framebuffer> swapchain_framebuffers;

	/// The renderpass description.
	vk::RenderPass render_pass;

	/// The graphics pipeline.
	vk::Pipeline pipeline;

	/**
	 * The pipeline layout for resources.
	 * Not used in this sample, but we still need to provide a dummy one.
	 */
	 //vk::PipelineLayout pipeline_layout;

	 /// The debug report callback.
	vk::DebugReportCallbackEXT debug_callback;

	/// A set of semaphores that can be reused.
	std::vector<vk::Semaphore> recycled_semaphores;

	/// A set of per-frame data.
	std::vector<PerFrame> per_frame;

	struct UploadContext {
		vk::Fence _uploadFence;
		vk::CommandPool _commandPool;
	}m_UploadContext;

	vk::CommandPool GetCommandPool() const {
		if (!m_UploadContext._commandPool) {
			//	vk::CommandPoolCreateInfo cmdPoolInfo;
			//	cmdPoolInfo.queueFamilyIndex = 0;//TODO: double check!queueIndices.graphics;
			//	cmdPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
			//	auto _commandPool = device.createCommandPool(cmdPoolInfo);
			//	//m_UploadContext._commandPool = *const_cast<  vk::CommandPool*>(&_commandPool);
		}
		return m_UploadContext._commandPool;
	}
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::PipelineLayout pipelineLayout;
	vk::DescriptorPool descriptorPool;
	vk::DescriptorSet descriptorSet;
};
struct AllocatedBuffer {
	VkBuffer _buffer;
	VmaAllocation _allocation;
};

VulkanContext Context;
GLFWwindow* WindowHandle;
VmaAllocator  Allocator;
vk::PhysicalDeviceMemoryProperties DeviceMemoryProperties;
vk::RenderPassBeginInfo  RenderPassBeginInfo;
Image DepthStencil;
MeshManager meshManager;
UniformBuffer* Uniform;

std::vector<vk::ClearValue> ClearValues;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

void InitInstance(const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers);
void InitLogger();
bool ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available);
bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available);
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type, uint64_t object, size_t location, int32_t message_code, const char* layer_prefix, const char* message, void* user_data);
std::vector<const char*> get_optimal_validation_layers(const std::vector<vk::LayerProperties>& supported_instance_layers);
std::string LoadFile(const std::string& path)
{
	std::ifstream stream(path);
	return  std::string((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
}
vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice);
void InitDevice(const std::vector<const char*>& required_device_extensions);
void TeardownPerFrame(PerFrame& per_frame);
void  InitAllocator();
void InitRenderPass();
void SetupDepthStencil();
 
vk::CommandBuffer createCommandBuffer(vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);
Image CreateImage(vk::ImageCreateInfo& imageCreateInfo, const vk::MemoryPropertyFlags& memoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal);
void FlushCommandBuffer(vk::CommandBuffer& commandBuffer);
void WithPrimaryCommandBuffer(const std::function<void(const vk::CommandBuffer& commandBuffer)>& f);
void  InitAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = Context.gpu;
	allocatorInfo.device = Context.device;
	allocatorInfo.instance = Context.instance;
	vmaCreateAllocator(&allocatorInfo, &Allocator);
}
vk::Bool32 GetMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& properties, uint32_t* typeIndex)
{
	for (uint32_t i = 0; i < 32; i++) {
		if ((typeBits & 1) == 1) {
			if ((DeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				*typeIndex = i;
				return VK_TRUE;
			}
		}
		typeBits >>= 1;
	}
	return VK_FALSE;
}
uint32_t GetMemoryType(uint32_t typeBits, const vk::MemoryPropertyFlags& properties)
{
	uint32_t result = 0;
	if (VK_FALSE == GetMemoryType(typeBits, properties, &result)) {
		throw std::runtime_error("Unable to find memory type " + vk::to_string(properties));
		// todo : throw error
	}
	return result;
}
Image CreateImage(vk::ImageCreateInfo& imageCreateInfo, const vk::MemoryPropertyFlags& memoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal);
void SetImageLayout(vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);
void SetImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);
void SetupRenderPassBeginInfo();
void  InitDescriptorPool();
void InitDescriptorSetLayout();
void  SetupDescriptorSet();
void InitPipeline();
void InitFrameBuffers();
void StageBuffer(Buffer& allocBuffer, vk::BufferUsageFlagBits usageFlags, VmaMemoryUsage memoryUsage);


vk::AccessFlags AccessFlagsForLayout(vk::ImageLayout layout)
{
	switch (layout) {
	case vk::ImageLayout::ePreinitialized:
		return vk::AccessFlagBits::eHostWrite;
	case vk::ImageLayout::eTransferDstOptimal:
		return vk::AccessFlagBits::eTransferWrite;
	case vk::ImageLayout::eTransferSrcOptimal:
		return vk::AccessFlagBits::eTransferRead;
	case vk::ImageLayout::eColorAttachmentOptimal:
		return vk::AccessFlagBits::eColorAttachmentWrite;
	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	case vk::ImageLayout::eShaderReadOnlyOptimal:
		return vk::AccessFlagBits::eShaderRead;
	default:
		return vk::AccessFlags();
	}
}
vk::PipelineStageFlags PipelineStageForLayout(vk::ImageLayout layout)
{
	switch (layout)
	{
	case vk::ImageLayout::eTransferDstOptimal:
	case vk::ImageLayout::eTransferSrcOptimal:
		return vk::PipelineStageFlagBits::eTransfer;

	case vk::ImageLayout::eColorAttachmentOptimal:
		return vk::PipelineStageFlagBits::eColorAttachmentOutput;

	case vk::ImageLayout::eDepthStencilAttachmentOptimal:
		return vk::PipelineStageFlagBits::eEarlyFragmentTests;

	case vk::ImageLayout::eShaderReadOnlyOptimal:
		return vk::PipelineStageFlagBits::eFragmentShader;

	case vk::ImageLayout::ePreinitialized:
		return vk::PipelineStageFlagBits::eHost;

	case vk::ImageLayout::eUndefined:
		return vk::PipelineStageFlagBits::eTopOfPipe;

	default:
		return vk::PipelineStageFlagBits::eBottomOfPipe;
	}
};
void TeardownPerFrame(PerFrame& per_frame);
void InitPerFrame(PerFrame& per_frame);
void SetupRenderPassBeginInfo();
void InitVertices();


int main()
{
	vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine",
		VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_2);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	WindowHandle = glfwCreateWindow(Width, Height, "Vulkan 101", nullptr, nullptr);

	InitInstance({ VK_KHR_SURFACE_EXTENSION_NAME }, {});
	SelectPhysicalDeviceAndInstance();
	std::string vertexSource = LoadFile("../Assets/shaders/triangle.vert");
	std::string fragmentSource = LoadFile("../Assets/shaders/triangle.frag");




}

void InitLogger()
{
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
#ifdef VKB_DEBUG
	logger->set_level(spdlog::level::debug);
#else
	logger->set_level(spdlog::level::info);
#endif

	logger->set_pattern(LOGGER_FORMAT);
	spdlog::set_default_logger(logger);
	LOGI("Logger initialized");
}
void InitInstance(const std::vector<const char*>& required_instance_extensions, const std::vector<const char*>& required_validation_layers)
{
	static vk::DynamicLoader  dl;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

	std::vector<vk::ExtensionProperties> instance_extensions = vk::enumerateInstanceExtensionProperties();
	std::vector<const char*> active_instance_extensions(required_instance_extensions);

#pragma region platform
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	active_instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	active_instance_extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
	active_instance_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	active_instance_extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	active_instance_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	active_instance_extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	active_instance_extensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
	active_instance_extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#else
#	pragma error Platform not supported
#endif
#pragma endregion 

	if (!ValidateExtensions(active_instance_extensions, instance_extensions))
	{
		throw std::runtime_error("Required instance extensions are missing.");
	}

	std::vector<vk::LayerProperties> supported_validation_layers = vk::enumerateInstanceLayerProperties();

	std::vector<const char*> requested_validation_layers(required_validation_layers);

#ifdef VKB_VALIDATION_LAYERS
	// Determine the optimal validation layers to enable that are necessary for useful debugging
	std::vector<const char*> optimal_validation_layers = get_optimal_validation_layers(supported_validation_layers);
	requested_validation_layers.insert(requested_validation_layers.end(), optimal_validation_layers.begin(), optimal_validation_layers.end());
#endif

	//todo use spdlog!
	if (ValidateLayers(requested_validation_layers, supported_validation_layers))
	{
		//fmt::print("Enabled Validation Layers:");
		for (const auto& layer : requested_validation_layers)
		{
			LOGI(layer);
		}
	}
	else
	{
		throw std::runtime_error("Required validation layers are missing.");
	}
	vk::ApplicationInfo app("HPP Vulkan 2", {}, "Ported from Vulkan Samples", VK_MAKE_VERSION(1, 0, 0));

	vk::InstanceCreateInfo instance_info({}, &app, requested_validation_layers, active_instance_extensions);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	vk::DebugReportCallbackCreateInfoEXT debug_report_create_info(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning, debug_callback);

	instance_info.pNext = &debug_report_create_info;
#endif

	// Create the Vulkan instance
	Context.instance = vk::createInstance(instance_info);

	// initialize function pointers for instance
	VULKAN_HPP_DEFAULT_DISPATCHER.init(Context.instance);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	Context.debug_callback = Context.instance.createDebugReportCallbackEXT(debug_report_create_info);
#endif
}
bool ValidateExtensions(const std::vector<const char*>& required, const std::vector<vk::ExtensionProperties>& available)
{
	// inner find_if gives true if the extension was not found
	// outer find_if gives true if none of the extensions were not found, that is if all extensions were found
	auto requiredButNotFoundIt = std::find_if(required.begin(),
		required.end(),
		[&available](auto extension) {
			return std::find_if(available.begin(),
				available.end(),
				[&extension](auto const& ep) {
					return strcmp(ep.extensionName, extension) == 0;
				}) == available.end();
		});

	if (requiredButNotFoundIt != required.end())
	{
		LOGE("Validation Layer {} not found", *requiredButNotFoundIt);
	}
	return  requiredButNotFoundIt == required.end();
}
bool ValidateLayers(const std::vector<const char*>& required, const std::vector<vk::LayerProperties>& available)
{
	// inner find_if returns true if the layer was not found
	// outer find_if returns iterator to the not found layer, if any
	auto requiredButNotFoundIt = std::find_if(required.begin(),
		required.end(),
		[&available](auto layer) {
			return std::find_if(available.begin(),
				available.end(),
				[&layer](auto const& lp) {
					return strcmp(lp.layerName, layer) == 0;
				}) == available.end();
		});
	if (requiredButNotFoundIt != required.end())
	{
		fmt::print("Validation Layer {} not found", *requiredButNotFoundIt);
	}
	return (requiredButNotFoundIt == required.end());
}
std::vector<const char*> get_optimal_validation_layers(const std::vector<vk::LayerProperties>& supported_instance_layers)
{
	std::vector<std::vector<const char*>> validation_layer_priority_list =
	{
		// The preferred validation layer is "VK_LAYER_KHRONOS_validation"
		{"VK_LAYER_KHRONOS_validation"},

		// Otherwise we fallback to using the LunarG meta layer
		{"VK_LAYER_LUNARG_standard_validation"},

		// Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
		{
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_GOOGLE_unique_objects",
		},

		// Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
		{"VK_LAYER_LUNARG_core_validation"} };

	for (auto& validation_layers : validation_layer_priority_list)
	{
		if (ValidateLayers(validation_layers, supported_instance_layers))
		{
			return validation_layers;
		}

		LOGW("Couldn't enable validation layers (see log for error) - falling back");
	}

	// Else return nothing
	return {};
}
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
/// @brief A debug callback called from Vulkan validation layers.
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT type,
	uint64_t object, size_t location, int32_t message_code,
	const char* layer_prefix, const char* message, void* user_data)
{
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		LOGE("Validation Layer: Error: {}: {}", layer_prefix, message);
	}
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		LOGW("Validation Layer: Warning: {}: {}", layer_prefix, message);
	}
	else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		LOGI("Validation Layer: Performance warning: {}: {}", layer_prefix, message);
	}
	else
	{
		LOGI("Validation Layer: Information: {}: {}", layer_prefix, message);
	}
	return VK_FALSE;
}
#endif

void  SelectPhysicalDeviceAndInstance()
{
	std::vector<vk::PhysicalDevice> gpus = Context.instance.enumeratePhysicalDevices();

	for (size_t i = 0; i < gpus.size() && (Context.graphics_queue_index < 0); i++)
	{
		Context.gpu = gpus[i];

		std::vector<vk::QueueFamilyProperties> queue_family_properties = Context.gpu.getQueueFamilyProperties();

		if (queue_family_properties.empty())
		{
			throw std::runtime_error("No queue family found.");
		}

		if (Context.surface)
		{
			Context.instance.destroySurfaceKHR(Context.surface);
		}
		Context.surface = CreateSurface(Context.instance, Context.gpu);

		for (uint32_t j = 0; j < static_cast<uint32_t>(queue_family_properties.size()); j++)
		{
			vk::Bool32 supports_present = Context.gpu.getSurfaceSupportKHR(j, Context.surface);

			// Find a queue family which supports graphics and presentation.
			if ((queue_family_properties[j].queueFlags & vk::QueueFlagBits::eGraphics) && supports_present)
			{
				Context.graphics_queue_index = j;
				break;
			}
		}
	}

	if (Context.graphics_queue_index < 0)
	{
		LOGE("Did not find suitable queue which supports graphics and presentation.");
	}
}

vk::SurfaceKHR CreateSurface(vk::Instance instance, vk::PhysicalDevice)
{
	if (!instance || !WindowHandle)
	{
		return nullptr;
	}

	VkSurfaceKHR surface;

	VkResult errCode = glfwCreateWindowSurface(instance, WindowHandle, NULL, &surface);

	if (errCode != VK_SUCCESS)
	{
		return nullptr;
	}

	return surface;
}
void InitDevice(const std::vector<const char*>& required_device_extensions)
{
	LOGI("Initializing vulkan device.");
	std::vector<vk::ExtensionProperties> device_extensions = Context.gpu.enumerateDeviceExtensionProperties();

	if (!ValidateExtensions(required_device_extensions, device_extensions))
	{
		throw std::runtime_error("Required device extensions are missing, will try without.");
	}

	float queue_priority = 1.0f;

	// Create one queue
	vk::DeviceQueueCreateInfo queue_info({}, Context.graphics_queue_index, 1, &queue_priority);

	vk::DeviceCreateInfo device_info({}, queue_info, {}, required_device_extensions);

	Context.device = Context.gpu.createDevice(device_info);
	// initialize function pointers for device
	VULKAN_HPP_DEFAULT_DISPATCHER.init(Context.device);

	Context.queue = Context.device.getQueue(Context.graphics_queue_index, 0);

}
void InitPerFrame(PerFrame& per_frame)
{
	per_frame.queue_submit_fence = Context.device.createFence({ vk::FenceCreateFlagBits::eSignaled });

	vk::CommandPoolCreateInfo cmd_pool_info(vk::CommandPoolCreateFlagBits::eTransient, Context.graphics_queue_index);
	per_frame.primary_command_pool = Context.device.createCommandPool(cmd_pool_info);

	vk::CommandBufferAllocateInfo cmd_buf_info(per_frame.primary_command_pool, vk::CommandBufferLevel::ePrimary, 1);
	per_frame.primary_command_buffer = Context.device.allocateCommandBuffers(cmd_buf_info).front();

	per_frame.device = Context.device;
	per_frame.queue_index = Context.graphics_queue_index;
}
void InitSwapchain()
{
	vk::SurfaceCapabilitiesKHR surface_properties = Context.gpu.getSurfaceCapabilitiesKHR(Context.surface);

	std::vector<vk::SurfaceFormatKHR> formats = Context.gpu.getSurfaceFormatsKHR(Context.surface);

	vk::SurfaceFormatKHR format;
	if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined)
	{
		// There is no preferred format, so pick a default one
		format = formats[0];
		format.format = vk::Format::eB8G8R8A8Unorm;
	}
	else
	{
		if (formats.empty())
		{
			throw std::runtime_error("Surface has no formats.");
		}

		format.format = vk::Format::eUndefined;
		for (auto& candidate : formats)
		{
			switch (candidate.format)
			{
			case vk::Format::eR8G8B8A8Unorm:
			case vk::Format::eB8G8R8A8Unorm:
			case vk::Format::eA8B8G8R8UnormPack32:
				format = candidate;
				break;

			default:
				break;
			}

			if (format.format != vk::Format::eUndefined)
			{
				break;
			}
		}

		if (format.format == vk::Format::eUndefined)
		{
			format = formats[0];
		}
	}

	vk::Extent2D swapchain_size;
	if (surface_properties.currentExtent.width == 0xFFFFFFFF)
	{
		swapchain_size.width = Context.swapchain_dimensions.width;
		swapchain_size.height = Context.swapchain_dimensions.height;
	}
	else
	{
		swapchain_size = surface_properties.currentExtent;
	}

	// FIFO must be supported by all implementations.
	vk::PresentModeKHR swapchain_present_mode = vk::PresentModeKHR::eFifo;

	// Determine the number of vk::Image's to use in the swapchain.
	// Ideally, we desire to own 1 image at a time, the rest of the images can
	// either be rendered to and/or being queued up for display.
	uint32_t desired_swapchain_images = surface_properties.minImageCount + 1;
	if ((surface_properties.maxImageCount > 0) && (desired_swapchain_images > surface_properties.maxImageCount))
	{
		// Application must settle for fewer images than desired.
		desired_swapchain_images = surface_properties.maxImageCount;
	}

	// Figure out a suitable surface transform.
	vk::SurfaceTransformFlagBitsKHR pre_transform =
		(surface_properties.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) ? vk::SurfaceTransformFlagBitsKHR::eIdentity : surface_properties.currentTransform;

	vk::SwapchainKHR old_swapchain = Context.swapchain;

	// Find a supported composite type.
	vk::CompositeAlphaFlagBitsKHR composite = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	}
	else if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::eInherit;
	}
	else if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
	}
	else if (surface_properties.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
	{
		composite = vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
	}

	//swapchain_size.width = swapchain_size.width == 0 ? 1 : swapchain_size.width ;
	//swapchain_size.height = swapchain_size.height == 0 ? 1 : swapchain_size.height ;

	vk::SwapchainCreateInfoKHR info;
	info.surface = Context.surface;
	info.minImageCount = desired_swapchain_images;
	info.imageFormat = format.format;
	info.imageColorSpace = format.colorSpace;
	info.imageExtent.width = swapchain_size.width;
	info.imageExtent.height = swapchain_size.height;
	info.imageArrayLayers = 1;
	info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	info.imageSharingMode = vk::SharingMode::eExclusive;
	info.preTransform = pre_transform;
	info.compositeAlpha = composite;
	info.presentMode = swapchain_present_mode;
	info.clipped = true;
	info.oldSwapchain = old_swapchain;

	Context.swapchain = Context.device.createSwapchainKHR(info);

	if (old_swapchain)
	{
		for (vk::ImageView image_view : Context.swapchain_image_views)
		{
			Context.device.destroyImageView(image_view);
		}

		size_t image_count = Context.device.getSwapchainImagesKHR(old_swapchain).size();

		for (size_t i = 0; i < image_count; i++)
		{
			TeardownPerFrame(Context.per_frame[i]);
		}

		Context.swapchain_image_views.clear();

		Context.device.destroySwapchainKHR(old_swapchain);
	}

	Context.swapchain_dimensions = { swapchain_size.width, swapchain_size.height, format.format };

	/// The swapchain images.
	std::vector<vk::Image> swapchain_images = Context.device.getSwapchainImagesKHR(Context.swapchain);
	size_t                 image_count = swapchain_images.size();

	// Initialize per-frame resources.
	// Every swapchain image has its own command pool and fence manager.
	// This makes it very easy to keep track of when we can reset command buffers and such.
	Context.per_frame.clear();
	Context.per_frame.resize(image_count);

	for (size_t i = 0; i < image_count; i++)
	{
		InitPerFrame(Context.per_frame[i]);
	}

	vk::ImageViewCreateInfo view_info;
	view_info.viewType = vk::ImageViewType::e2D;
	view_info.format = Context.swapchain_dimensions.format;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.layerCount = 1;
	view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	view_info.components.r = vk::ComponentSwizzle::eR;
	view_info.components.g = vk::ComponentSwizzle::eG;
	view_info.components.b = vk::ComponentSwizzle::eB;
	view_info.components.a = vk::ComponentSwizzle::eA;
	for (size_t i = 0; i < image_count; i++)
	{
		// Create an image view which we can render into.
		view_info.image = swapchain_images[i];

		Context.swapchain_image_views.push_back(Context.device.createImageView(view_info));
	}

	//upload Context
	vk::CommandPoolCreateInfo cmdPoolInfoUpload(vk::CommandPoolCreateFlagBits::eTransient, Context.graphics_queue_index);
	Context.m_UploadContext._commandPool = Context.device.createCommandPool(cmdPoolInfoUpload);

	vk::FenceCreateInfo uploadFenceCreateInfo;
	Context.m_UploadContext._uploadFence = Context.device.createFence(uploadFenceCreateInfo);


}

void TeardownPerFrame(PerFrame& per_frame)
{
	if (per_frame.queue_submit_fence)
	{
		Context.device.destroyFence(per_frame.queue_submit_fence);

		per_frame.queue_submit_fence = nullptr;
	}

	if (per_frame.primary_command_buffer)
	{
		Context.device.freeCommandBuffers(per_frame.primary_command_pool, per_frame.primary_command_buffer);

		per_frame.primary_command_buffer = nullptr;
	}

	if (per_frame.primary_command_pool)
	{
		Context.device.destroyCommandPool(per_frame.primary_command_pool);

		per_frame.primary_command_pool = nullptr;
	}

	if (per_frame.swapchain_acquire_semaphore)
	{
		Context.device.destroySemaphore(per_frame.swapchain_acquire_semaphore);

		per_frame.swapchain_acquire_semaphore = nullptr;
	}

	if (per_frame.swapchain_release_semaphore)
	{
		Context.device.destroySemaphore(per_frame.swapchain_release_semaphore);

		per_frame.swapchain_release_semaphore = nullptr;
	}

	per_frame.device = nullptr;
	per_frame.queue_index = -1;
}


void SetImageLayout(vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout)
{
	WithPrimaryCommandBuffer([&](const auto& commandBuffer) {
		SetImageLayout(commandBuffer, image, aspectMask, oldImageLayout, newImageLayout);
		});
}

void SetImageLayout(vk::CommandBuffer cmdbuffer,
	vk::Image image,
	vk::ImageAspectFlags aspectMask,
	vk::ImageLayout oldImageLayout,
	vk::ImageLayout newImageLayout)
{
	vk::ImageSubresourceRange subresourceRange;
	subresourceRange.aspectMask = aspectMask;
	subresourceRange.levelCount = 1;
	subresourceRange.layerCount = 1;
	SetImageLayout(cmdbuffer, image, oldImageLayout, newImageLayout, subresourceRange);
}

void SetImageLayout(vk::CommandBuffer cmdbuffer,
	vk::Image image,
	vk::ImageLayout oldImageLayout,
	vk::ImageLayout newImageLayout,
	vk::ImageSubresourceRange subresourceRange)
{
	// Create an image barrier object
	vk::ImageMemoryBarrier imageMemoryBarrier;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;
	imageMemoryBarrier.srcAccessMask = AccessFlagsForLayout(oldImageLayout);
	imageMemoryBarrier.dstAccessMask = AccessFlagsForLayout(newImageLayout);
	vk::PipelineStageFlags srcStageMask = PipelineStageForLayout(oldImageLayout);
	vk::PipelineStageFlags destStageMask = PipelineStageForLayout(newImageLayout);
	// Put barrier on top
	// Put barrier inside setup command buffer
	cmdbuffer.pipelineBarrier(srcStageMask, destStageMask, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);
}

void WithPrimaryCommandBuffer(const std::function<void(const vk::CommandBuffer& commandBuffer)>& f) {
	vk::CommandBuffer commandBuffer = createCommandBuffer(vk::CommandBufferLevel::ePrimary);
	commandBuffer.begin(vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	f(commandBuffer);
	commandBuffer.end();
	FlushCommandBuffer(commandBuffer);
	Context.device.freeCommandBuffers(Context.GetCommandPool(), commandBuffer);
}
void FlushCommandBuffer(vk::CommandBuffer& commandBuffer)
{
	if (!commandBuffer) {
		return;
	}
	Context.queue.submit(vk::SubmitInfo{ 0, nullptr, nullptr, 1, &commandBuffer }, vk::Fence());
	Context.queue.waitIdle();
	Context.device.waitIdle();
}

Image CreateImage(vk::ImageCreateInfo& imageCreateInfo, const vk::MemoryPropertyFlags& memoryPropertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal)
{
	auto& device = Context.device;
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Image result;


	VK_CHECK(vmaCreateImage(Allocator,
		reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo),
		&dimg_allocinfo, reinterpret_cast<VkImage*>(&result.image),
		&DepthStencil.allocation, nullptr));


	return result;
}

vk::CommandBuffer createCommandBuffer(vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary)
{
	vk::CommandBuffer cmdBuffer;
	vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
	cmdBufAllocateInfo.commandPool = Context.GetCommandPool();
	cmdBufAllocateInfo.level = level;
	cmdBufAllocateInfo.commandBufferCount = 1;
	cmdBuffer = Context.device.allocateCommandBuffers(cmdBufAllocateInfo)[0];
	return cmdBuffer;
}


void SetupDepthStencil()
{
	auto depthFormat = vk::Format::eD32SfloatS8Uint; //TODO: QUERY AND CHOOSE ONE

	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
	vk::ImageCreateInfo depthStencilCreateInfo;
	depthStencilCreateInfo.imageType = vk::ImageType::e2D;

	vk::Extent3D ext = { (uint32_t)Context.swapchain_dimensions.width, Context.swapchain_dimensions.height, 1u };
	depthStencilCreateInfo.extent = ext;
	depthStencilCreateInfo.format = depthFormat;
	depthStencilCreateInfo.mipLevels = 1;
	depthStencilCreateInfo.arrayLayers = 1;
	depthStencilCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;
	DepthStencil = CreateImage(depthStencilCreateInfo);

	SetImageLayout(DepthStencil.image, aspect, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::ImageViewCreateInfo depthStencilView;
	depthStencilView.viewType = vk::ImageViewType::e2D;
	depthStencilView.format = depthFormat;
	depthStencilView.subresourceRange.aspectMask = aspect;
	depthStencilView.subresourceRange.levelCount = 1;
	depthStencilView.subresourceRange.layerCount = 1;
	depthStencilView.image = DepthStencil.image;
	DepthStencil.view = Context.device.createImageView(depthStencilView);
}
void  LoadMeshes()
{
#pragma region
	//Buffer vb;
	//vb.AddAttribute(PositionAttribute{});
	//vb.AddAttribute(ColorAttribute{});
	//vb.Finalize();
	//PositionAttribute v0p = PositionAttribute{ 1.0f, 1.0f, 0.0f };
	//ColorAttribute v0c = ColorAttribute{ 1.0f, 0.0f, 0.0f };
	//PositionAttribute v1p = PositionAttribute{ -1.0f, 1.0f, 0.0f };
	//ColorAttribute v1c = ColorAttribute{ 0.0f, 1.0f, 0.0f };
	//PositionAttribute v2p = PositionAttribute{ 0.0f, -1.0f, 0.0f };
	//ColorAttribute v2c = ColorAttribute{ 0.0f, 0.0f, 1.0f };
	//vb.AddElement< PositionAttribute>(1.0f, 1.0f, 0.0f);
	//vb.AddElement< ColorAttribute>(1.0f, 0.0f, 0.0f);
	//vb.AddElement< PositionAttribute>(-1.0f, 1.0f, 0.0f);
	//vb.AddElement< ColorAttribute>(0.0f, 1.0f, 0.0f);
	//vb.AddElement< PositionAttribute>(0.0f, -1.0f, 0.0f);
	//vb.AddElement< ColorAttribute>(0.0f, 0.0f, 1.0f);
	//vb.AddElement< BoneWeightAttribute>(0.0f, 0.0f, 1.0f );
	//
	//auto a1 = vb.GetAttribute<PositionAttribute>(0);
	//auto b1 = vb.GetAttribute<ColorAttribute>(0);
	//auto a2 = vb.GetAttribute<PositionAttribute>(1);
	//auto b2 = vb.GetAttribute<ColorAttribute>(1);
	//auto a3 = vb.GetAttribute<PositionAttribute>(2);
	//auto b3 = vb.GetAttribute<ColorAttribute>(2);
	//auto b4 = vb.GetAttribute<ColorAttribute>(3);
	//
	//IndexBuffer ib;
	//ib.AddElement< IndexAttribute>( 0 , 1 , 2 );
#pragma endregion

	meshManager.LoadFromFile("../Assets/monke.glb", aiPostProcessSteps::aiProcess_Triangulate);
}

void InitRenderPass()
{
	std::vector<vk::AttachmentDescription> attachments;
	attachments.resize(2);

	// Backbuffer format.
	attachments[0].format = Context.swapchain_dimensions.format;
	// Not multisampled.
	attachments[0].samples = vk::SampleCountFlagBits::e1;
	// When starting the frame, we want tiles to be cleared.
	attachments[0].loadOp = vk::AttachmentLoadOp::eClear;
	// When ending the frame, we want tiles to be written out.
	attachments[0].storeOp = vk::AttachmentStoreOp::eStore;
	// Don't care about stencil since we're not using it.
	attachments[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	attachments[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;

	// The image layout will be undefined when the render pass begins.
	attachments[0].initialLayout = vk::ImageLayout::eUndefined;
	// After the render pass is complete, we will transition to ePresentSrcKHR layout.
	attachments[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

	//depth attachment
	attachments[1].format = vk::Format::eD32SfloatS8Uint; //TODO: set this dynamically!
	attachments[1].loadOp = vk::AttachmentLoadOp::eClear;
	attachments[1].storeOp = vk::AttachmentStoreOp::eDontCare;
	attachments[1].stencilLoadOp = vk::AttachmentLoadOp::eClear;
	attachments[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	attachments[1].initialLayout = vk::ImageLayout::eUndefined;
	attachments[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;




	// We have one subpass. This subpass has one color attachment.
	// While executing this subpass, the attachment will be in attachment optimal layout.
	//vk::AttachmentReference color_ref(0, vk::ImageLayout::eColorAttachmentOptimal);
	std::vector<vk::AttachmentReference> colorAttachmentReferences;
	{
		vk::AttachmentReference colorReference;
		colorReference.attachment = 0;
		colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;
		colorAttachmentReferences.push_back(colorReference);
	}

	vk::AttachmentReference depthReference;
	depthReference.attachment = 1;
	depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

	// We will end up with two transitions.
	// The first one happens right before we start subpass #0, where
	// eUndefined is transitioned into eColorAttachmentOptimal.
	// The final layout in the render pass attachment states ePresentSrcKHR, so we
	// will get a final transition from eColorAttachmentOptimal to ePresetSrcKHR.
	//vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, {}, color_ref,nullptr, &depthReference);
	std::vector<vk::SubpassDescription> subpasses{
	  {
		  {}, vk::PipelineBindPoint::eGraphics,
		  // Input attachment references
		  0, nullptr,
		  // Color / resolve attachment references
		  (uint32_t)colorAttachmentReferences.size(), colorAttachmentReferences.data(), nullptr,
		  // Depth stecil attachment reference,
		  &depthReference,
		  // Preserve attachments
		  0, nullptr
	  },
	};
	using vPSFB = vk::PipelineStageFlagBits;
	using vAFB = vk::AccessFlagBits;
	std::vector<vk::SubpassDependency> subpassDependencies{
		{
			0, VK_SUBPASS_EXTERNAL,
			vPSFB::eColorAttachmentOutput, vPSFB::eBottomOfPipe,
			vAFB::eColorAttachmentRead | vAFB::eColorAttachmentWrite, vAFB::eMemoryRead,
			vk::DependencyFlagBits::eByRegion
		},
		{
			VK_SUBPASS_EXTERNAL, 0,
			vPSFB::eBottomOfPipe, vPSFB::eColorAttachmentOutput,
			vAFB::eMemoryRead, vAFB::eColorAttachmentRead | vAFB::eColorAttachmentWrite,
			vk::DependencyFlagBits::eByRegion
		},
	};
	// Create a dependency to external events.
	// We need to wait for the WSI semaphore to signal.
	// Only pipeline stages which depend on eColorAttachmentOutput will
	// actually wait for the semaphore, so we must also wait for that pipeline stage.
	//old
	//vk::SubpassDependency dependency;
	//dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	//dependency.dstSubpass = 0;
	//dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	//dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	//
	//// Since we changed the image layout, we need to make the memory visible to
	//// color attachment to modify.
	//dependency.srcAccessMask = {};
	//dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	// Finally, create the renderpass.
	//vk::RenderPassCreateInfo rp_info({}, attachments, subpass, dependency);
	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachmentCount = (uint32_t)attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = (uint32_t)subpasses.size();
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = (uint32_t)subpassDependencies.size();
	renderPassInfo.pDependencies = subpassDependencies.data();

	Context.render_pass = Context.device.createRenderPass(renderPassInfo);
}

void SetupRenderPassBeginInfo()
{
	ClearValues.clear();
	ClearValues.push_back(vk::ClearValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f}));
	ClearValues.push_back(vk::ClearDepthStencilValue{ 1.0f, 0 });

	RenderPassBeginInfo = vk::RenderPassBeginInfo();
	RenderPassBeginInfo.renderPass = Context.render_pass;
	RenderPassBeginInfo.renderArea.extent = vk::Extent2D{ Context.swapchain_dimensions.width,Context.swapchain_dimensions.height };
	RenderPassBeginInfo.clearValueCount = (uint32_t)ClearValues.size();
	RenderPassBeginInfo.pClearValues = ClearValues.data();
}
void SetupRenderPassBeginInfo()
{
	ClearValues.clear();
	ClearValues.push_back(vk::ClearValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f}));
	ClearValues.push_back(vk::ClearDepthStencilValue{ 1.0f, 0 });

	RenderPassBeginInfo = vk::RenderPassBeginInfo();
	RenderPassBeginInfo.renderPass = Context.render_pass;
	RenderPassBeginInfo.renderArea.extent = vk::Extent2D{ Context.swapchain_dimensions.width,Context.swapchain_dimensions.height };
	RenderPassBeginInfo.clearValueCount = (uint32_t)ClearValues.size();
	RenderPassBeginInfo.pClearValues = ClearValues.data();
}

void InitDescriptorPool()
{
	// We need to tell the API the number of max. requested descriptors per type
	vk::DescriptorPoolSize typeCounts[1];
	// This example only uses one descriptor type (uniform buffer) and only
	// requests one descriptor of this type
	typeCounts[0].type = vk::DescriptorType::eUniformBuffer;
	typeCounts[0].descriptorCount = 1;
	// For additional types you need to add new entries in the type count list
	// E.g. for two combined image samplers :
	// typeCounts[1].type = vk::DescriptorType::eCombinedImageSampler;
	// typeCounts[1].descriptorCount = 2;

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes = typeCounts;
	// Set the max. number of sets that can be requested
	// Requesting descriptors beyond maxSets will result in an error
	descriptorPoolInfo.maxSets = 1;
	Context.descriptorPool = Context.device.createDescriptorPool(descriptorPoolInfo);
}

void InitDescriptorSetLayout()
{
	static vk::DescriptorSetLayoutBinding layoutBinding;
	layoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.binding = 0;

	static vk::DescriptorSetLayoutCreateInfo descriptorLayout;
	descriptorLayout.bindingCount = 1;
	descriptorLayout.pBindings = &layoutBinding;

	Context.descriptorSetLayout = Context.device.createDescriptorSetLayout(descriptorLayout, nullptr);

	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &Context.descriptorSetLayout;

	Context.pipelineLayout = Context.device.createPipelineLayout(pipelineLayoutCreateInfo);
}

void  SetupDescriptorSet()
{
	// Allocate a new descriptor set from the global descriptor pool
	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.descriptorPool = Context.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &Context.descriptorSetLayout;

	Context.descriptorSet = Context.device.allocateDescriptorSets(allocInfo)[0];


	vk::WriteDescriptorSet writeDescriptorSet;
	// Binding 0 : Uniform buffer
	writeDescriptorSet.dstSet = Context.descriptorSet;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = vk::DescriptorType::eUniformBuffer;
	writeDescriptorSet.pBufferInfo = &Uniform->m_DescrBufferInfo;
	// Binds this uniform buffer to binding point 0
	writeDescriptorSet.dstBinding = 0;


	//writeDescriptorSet.pBufferInfo = &descrBufferInfo;
	//writeDescriptorSet.setBufferInfo()
	Context.device.updateDescriptorSets(writeDescriptorSet, nullptr);
}

void InitPipeline()
{
	// Create a blank pipeline layout.
	// We are not binding any resources to the pipeline in this first sample.
	Context.pipelineLayout;// = Context.device.createPipelineLayout({});


	vk::PipelineVertexInputStateCreateInfo vertexInput;
	auto& triMesh = meshManager.GetMesh(0);
	auto bindingDescriptions = triMesh.m_VertexBuffer.GetVertexInputBindingDescriptions();
	vertexInput.setVertexBindingDescriptions({ (uint32_t)bindingDescriptions.size(), bindingDescriptions.data() });

	auto attributeDescriptions = triMesh.m_VertexBuffer.GetVertexInputAttributeDescriptions();
	vertexInput.setVertexAttributeDescriptions({ (uint32_t)attributeDescriptions.size(), attributeDescriptions.data() });

	// Specify we will use triangle lists to draw geometry.
	vk::PipelineInputAssemblyStateCreateInfo input_assembly({}, vk::PrimitiveTopology::eTriangleList);

	// Specify rasterization state.
	vk::PipelineRasterizationStateCreateInfo raster;
	raster.cullMode = vk::CullModeFlagBits::eNone;
	raster.frontFace = vk::FrontFace::eCounterClockwise;
	raster.lineWidth = 1.0f;

	// Our attachment will write to all color channels, but no blending is enabled.
	vk::PipelineColorBlendAttachmentState blend_attachment;
	blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

	vk::PipelineColorBlendStateCreateInfo blend({}, {}, {}, blend_attachment);

	// We will have one viewport and scissor box.
	vk::PipelineViewportStateCreateInfo viewport;
	viewport.viewportCount = 1;
	viewport.scissorCount = 1;

	// Disable all depth testing.
	vk::PipelineDepthStencilStateCreateInfo depth_stencil;
	depth_stencil.depthTestEnable = VK_TRUE;
	depth_stencil.depthWriteEnable = VK_TRUE;
	depth_stencil.depthCompareOp = vk::CompareOp::eLess;
	depth_stencil.depthBoundsTestEnable = VK_FALSE;
	depth_stencil.stencilTestEnable = VK_FALSE;

	// No multisampling.
	vk::PipelineMultisampleStateCreateInfo multisample({}, vk::SampleCountFlagBits::e1);

	// Specify that these states will be dynamic, i.e. not part of pipeline state object.
	std::array<vk::DynamicState, 2> dynamics{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamic({}, dynamics);


	std::string vertexSource = LoadFile("../Assets/shaders/triangle.vert");
	std::string fragmentSource = LoadFile("../Assets/shaders/triangle.frag");

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetOptimizationLevel(shaderc_optimization_level_performance);

	shaderc::SpvCompilationResult vertShaderResult =
		compiler.CompileGlslToSpv(vertexSource, shaderc_glsl_vertex_shader, "vertex shader", options);

	auto vertStatus = vertShaderResult.GetCompilationStatus();
	if (vertStatus != shaderc_compilation_status_success) { LOGE(vertShaderResult.GetErrorMessage()); }

	auto vertShaderCode = std::vector<uint32_t>{ vertShaderResult.cbegin(), vertShaderResult.cend() };
	auto vertSize = std::distance(vertShaderCode.begin(), vertShaderCode.end());
	auto vertShaderCreateInfo = vk::ShaderModuleCreateInfo{ {}, vertSize * sizeof(uint32_t), vertShaderCode.data() };
	auto vertexShaderModule = Context.device.createShaderModule(vertShaderCreateInfo);

	shaderc::SpvCompilationResult fragShaderModule = compiler.CompileGlslToSpv(fragmentSource, shaderc_glsl_fragment_shader, "fragment shader", options);
	if (fragShaderModule.GetCompilationStatus() != shaderc_compilation_status_success) 	  fmt::print(fragShaderModule.GetErrorMessage());

	auto fragShaderCode = std::vector<uint32_t>{ fragShaderModule.cbegin(), fragShaderModule.cend() };
	auto fragSize = std::distance(fragShaderCode.begin(), fragShaderCode.end());
	auto fragShaderCreateInfo = vk::ShaderModuleCreateInfo{ {}, fragSize * sizeof(uint32_t), fragShaderCode.data() };
	auto fragmentShaderModule = Context.device.createShaderModule(fragShaderCreateInfo);


	std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages{
	vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vertexShaderModule, "main"),
	vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, fragmentShaderModule, "main") };


	vk::GraphicsPipelineCreateInfo pipe({}, shader_stages);
	pipe.pVertexInputState = &vertexInput;
	pipe.pInputAssemblyState = &input_assembly;
	pipe.pRasterizationState = &raster;
	pipe.pColorBlendState = &blend;
	pipe.pMultisampleState = &multisample;
	pipe.pViewportState = &viewport;
	pipe.pDepthStencilState = &depth_stencil;
	pipe.pDynamicState = &dynamic;

	// We need to specify the pipeline layout and the render pass description up front as well.
	pipe.renderPass = Context.render_pass;
	pipe.setLayout(Context.pipelineLayout);

	Context.pipeline = Context.device.createGraphicsPipeline(nullptr, pipe).value;
	VkPipeline a1 = static_cast<VkPipeline>(Context.pipeline);
	VkPipelineLayout a2 = static_cast<VkPipelineLayout>(Context.pipelineLayout);
	//VkPipeline a1 = static_cast<VkPipeline>(Context.pipeline);
	// Pipeline is baked, we can delete the shader modules now.
	Context.device.destroyShaderModule(shader_stages[0].module);
	Context.device.destroyShaderModule(shader_stages[1].module);
}


void InitFrameBuffers()
{
	vk::Device device = Context.device;

	// Depth/Stencil attachment is the same for all frame buffers
	// Create framebuffer for each swapchain image view
	for (auto& image_view : Context.swapchain_image_views)
	{
		// Build the framebuffer.
		//old: vk::FramebufferCreateInfo fb_info({}, Context.render_pass, image_view, Context.swapchain_dimensions.width, Context.swapchain_dimensions.height, 1);
		vk::ImageView attachments[2];
		attachments[0] = image_view;
		attachments[1] = DepthStencil.view;

		vk::FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.renderPass = Context.render_pass;
		framebufferCreateInfo.attachmentCount = 2;
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.width = Context.swapchain_dimensions.width;
		framebufferCreateInfo.height = Context.swapchain_dimensions.height;
		framebufferCreateInfo.layers = 1;

		Context.swapchain_framebuffers.push_back(device.createFramebuffer(framebufferCreateInfo));
	}
}

void StageBuffer(Buffer& allocBuffer, vk::BufferUsageFlagBits usageFlags, VmaMemoryUsage memoryUsage)
{
	const size_t bufferSize = allocBuffer.GetBufferSize();

	vk::BufferCreateInfo stagingBufferInfo({}, bufferSize, vk::BufferUsageFlagBits::eTransferSrc);
	const VkBufferCreateInfo C_stagingBufferInfo = stagingBufferInfo;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	AllocatedBuffer stagingBuffer;

	VK_CHECK(vmaCreateBuffer(Allocator, reinterpret_cast<VkBufferCreateInfo*>(&stagingBufferInfo), &vmaallocInfo, &stagingBuffer._buffer, &stagingBuffer._allocation, nullptr));

	void* data;
	VK_CHECK(vmaMapMemory(Allocator, stagingBuffer._allocation, &data));
	memcpy(data, allocBuffer.buffer.data(), bufferSize);

	vmaUnmapMemory(Allocator, stagingBuffer._allocation);
	////////////////////////
	////  COPY 1 DONE!  ////
	////////////////////////

	//C++ style
	auto usage = usageFlags | vk::BufferUsageFlagBits::eTransferDst;
	auto vkBuffer_CI = vk::BufferCreateInfo{ {}, bufferSize, usage };
	VkBufferCreateInfo vertexBufferInfo = vkBuffer_CI; //convert!
	//let the VMA library know that this data should be gpu native	

	vmaallocInfo.usage = memoryUsage;
	VkBuffer vkBuff = allocBuffer.vkBuffer;

	VmaAllocationInfo allocationInfo{};
	VK_CHECK(vmaCreateBuffer(Allocator, &vertexBufferInfo, &vmaallocInfo, reinterpret_cast<VkBuffer*>(&allocBuffer.vkBuffer), &allocBuffer.vmaAllocation, &allocationInfo));
	allocBuffer.DeviceMemory = static_cast<vk::DeviceMemory>(allocationInfo.deviceMemory);

	allocBuffer.mapped_data = static_cast<uint8_t*>(allocationInfo.pMappedData);

	auto cmdBuffer_AI = vk::CommandBufferAllocateInfo{ Context.m_UploadContext._commandPool, vk::CommandBufferLevel::ePrimary, 1 };
	auto allocatedCmdBuffers = Context.device.allocateCommandBuffers(cmdBuffer_AI);
	vk::CommandBuffer cmdBuffer = allocatedCmdBuffers.front();

	auto cmdBuffer_BI = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	cmdBuffer.begin(cmdBuffer_BI);

	////////////////////////				lambda
	VkBufferCopy copy;
	copy.dstOffset = 0;
	copy.srcOffset = 0;
	copy.size = bufferSize;

	vkCmdCopyBuffer(cmdBuffer, stagingBuffer._buffer, allocBuffer.vkBuffer, 1, &copy);
	////////////////////////				lambda
	cmdBuffer.end();

	auto submitInfo = vk::SubmitInfo{ {}, {}, {1, &cmdBuffer} };

	Context.queue.submit(submitInfo, Context.m_UploadContext._uploadFence);
	Context.device.waitForFences(Context.m_UploadContext._uploadFence, VK_TRUE, 9999999999);
	Context.device.resetFences(Context.m_UploadContext._uploadFence);
	Context.device.resetCommandPool(Context.m_UploadContext._commandPool);

	vmaDestroyBuffer(Allocator, stagingBuffer._buffer, stagingBuffer._allocation);
}

void InitVertices()
{
	auto& triMesh = meshManager.GetMesh(0);
	StageBuffer(triMesh.m_VertexBuffer, vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_GPU_ONLY);
	StageBuffer(triMesh.m_IndexBuffer, vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_GPU_ONLY);
}