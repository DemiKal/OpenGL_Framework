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
#include <ShaderVulkan.h>

#define LOGGER_FORMAT "[%^%l%$] %v"
#define ROOT_PATH_SIZE 48
#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

#define LOGI(...) spdlog::info(__VA_ARGS__);
#define LOGW(...) spdlog::warn(__VA_ARGS__);
#define LOGE(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));
#define LOGD(...) spdlog::debug(__VA_ARGS__);

//VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

constexpr uint32_t Width = 1024;
constexpr uint32_t Height = 768;

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

VulkanContext Context;
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

int main()
{
	vk::ApplicationInfo appInfo("Hello Triangle", VK_MAKE_VERSION(1, 0, 0), "No Engine",
		VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_2);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* windowHandle = glfwCreateWindow(Width, Height, "Vulkan 101", nullptr, nullptr);
	
	InitInstance({ VK_KHR_SURFACE_EXTENSION_NAME }, {});
	std::string vertexSource = LoadFile("../Assets/shaders/triangle.vert");
	std::string fragmentSource = LoadFile("../Assets/shaders/triangle.frag");

	//Shaders::LoadShader(vertexSource);
	//Shaders::LoadShader(fragmentSource);

	ShaderVulkan::LoadShader(vertexSource);
	//shaderc::Compiler compiler;
	//shaderc::CompileOptions options;
	//options.SetOptimizationLevel(shaderc_optimization_level_performance);
	//
	//shaderc::SpvCompilationResult vertShaderResult =
	//	compiler.CompileGlslToSpv("asdasdsdsa", shaderc_glsl_vertex_shader, "vertex shader", options);


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
	Context.debug_callback =  Context.instance.createDebugReportCallbackEXT(debug_report_create_info);
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