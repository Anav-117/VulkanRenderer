#include "VulkanConfig.h"

#include <stdexcept>
#include <iostream>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return  bindingDescription;

	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {

		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;

	}
};

namespace vk {

	VkInstance instance;
	std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR surface;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	std::vector<VkPipelineLayout> pipelineLayout;
	std::vector<VkPipeline> graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	std::vector<std::vector<Vertex>> vertices;
	std::vector<std::vector<uint32_t>> indices;
	std::vector<VkBuffer> vertexBuffer;
	std::vector<VkDeviceMemory> vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkDescriptorPool descriptorPool;
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	bool FramebufferResized = false;

	std::vector<std::vector<VkBuffer>> uniformBuffers_Transformation;
	std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory_Transformation;
	std::vector<std::vector<void*>> uniformBuffersMapped_Transformation;
	VkDescriptorSetLayout descriptorSetLayout_Transformation;
	std::vector<std::vector<VkDescriptorSet>> descriptorSets_Transformation;

	std::vector<std::vector<VkBuffer>> uniformBuffers_Light;
	std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory_Light;
	std::vector<std::vector<void*>> uniformBuffersMapped_Light;
	VkDescriptorSetLayout descriptorSetLayout_Light;
	std::vector<std::vector<VkDescriptorSet>> descriptorSets_Light;

	std::vector<std::string> MODEL_PATH;
	std::vector<std::string> TEXTURE_PATH;

	int NUM_MODELS = 4;
	int NUM_SHADERS = 6;

	VkDevice device;
	std::vector<VkFence> inFlightFence;
	std::vector<VkSemaphore> imageAvailableSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphore;
	VkSwapchainKHR swapChain;
	std::vector<VkCommandBuffer> commandBuffer;
	VkQueue presentQueue;
	VkQueue graphicsQueue;
	VkExtent2D swapChainExtent;

	std::vector<VkImage> textureImage;
	std::vector<VkDeviceMemory> textureImageMemory;
	std::vector<VkImageView> textureImageView;
	std::vector<VkSampler> textureSampler;
 
	void createInstance();
	bool checkValidationLayerSupport();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface(GLFWwindow* window);
	void createSwapChain(GLFWwindow* window);
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout_Transformation();
	void createDescriptorSetLayout_Light();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void createDepthResources();
	void loadModel();
	void createVertexBuffer();
	void createIndexBuffer();
	//void createUniformBuffers(size_t size, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped);
	void createDescriptorPool();
	//void createDescriptorSets(std::vector<VkBuffer>& uniformBuffers);
	void createDescriptor_Transformation();
	void createDescriptor_Light();
	void createCommandBuffer();
	void createSyncObjects();
	void cleanupSwapChain();
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

}

struct QueueFamilyIndices {

	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

};

struct SwapChainSupportDetails {

	VkSurfaceCapabilitiesKHR capablities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}

}

bool vk::checkValidationLayerSupport() {

	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;

}

std::vector<const char*> getRequiredExtensions() {

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;

}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData) {
	
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	}

	return false;

}

void vk::setupDebugMessenger() {

	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;

	if (CreateDebugUtilsMessengerEXT(vk::instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Set Up Debug Messenger!");
	}

}


void vk::Init(GLFWwindow* window) {

	vk::createInstance();
	vk::setupDebugMessenger();
	vk::createSurface(window);
	vk::pickPhysicalDevice();
	vk::createLogicalDevice();
	vk::createSwapChain(window);
	vk::createImageViews();
	vk::createRenderPass();
	vk::createDescriptorSetLayout_Transformation();
	vk::createDescriptorSetLayout_Light();
	vk::createGraphicsPipeline();
	vk::createDepthResources();
	vk::createFramebuffers();
	vk::createCommandPool();
	vk::createTextureImage();
	vk::createTextureImageView();
	vk::createTextureSampler();
	vk::loadModel();
	vk::createVertexBuffer();
	vk::createIndexBuffer();
	//vk::createUniformBuffers(sizeof(Transformations), uniformBuffers_Transformation, uniformBuffersMemory_Transformation, uniformBuffersMapped_Transformation);
	//vk::createUniformBuffers(sizeof(Light), uniformBuffers_Light, uniformBuffersMemory_Light, uniformBuffersMapped_Light);
	vk::createDescriptorPool();
	//vk::createDescriptorSets(uniformBuffers_Transformation);
	//vk::createDescriptorSets(uniformBuffers_Light);
	vk::createDescriptor_Transformation();
	vk::createDescriptor_Light();
	vk::createCommandBuffer();
	vk::createSyncObjects();

}

void vk::Cleanup() {

	vk::cleanupSwapChain();

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		vkDestroySampler(vk::device, vk::textureSampler[i], nullptr);
		vkDestroyImageView(vk::device, vk::textureImageView[i], nullptr);
		vkDestroyImage(vk::device, vk::textureImage[i], nullptr);
		vkFreeMemory(vk::device, vk::textureImageMemory[i], nullptr);
	}

	for (size_t i = 0; i < vk::NUM_SHADERS; i++) {
		vkDestroyPipeline(vk::device, vk::graphicsPipeline[i], nullptr);
		vkDestroyPipelineLayout(vk::device, vk::pipelineLayout[i], nullptr);
	}
	vkDestroyRenderPass(vk::device, vk::renderPass, nullptr);

	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(vk::device, vk::imageAvailableSemaphore[i], nullptr);
		vkDestroySemaphore(vk::device, vk::renderFinishedSemaphore[i], nullptr);
		vkDestroyFence(vk::device, vk::inFlightFence[i], nullptr);
	}

	vkDestroyCommandPool(vk::device, vk::commandPool, nullptr);

	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		for (size_t j = 0; j < vk::NUM_MODELS; j++) {
			vkDestroyBuffer(vk::device, vk::uniformBuffers_Transformation[i][j], nullptr);
			vkFreeMemory(vk::device, vk::uniformBuffersMemory_Transformation[i][j], nullptr);
		}
	}

	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		for (size_t j = 0; j < vk::NUM_MODELS; j++) {
			vkDestroyBuffer(vk::device, vk::uniformBuffers_Light[i][j], nullptr);
			vkFreeMemory(vk::device, vk::uniformBuffersMemory_Light[i][j], nullptr);
		}
	}

	vkDestroyDescriptorPool(vk::device, vk::descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vk::device, vk::descriptorSetLayout_Transformation, nullptr);
	vkDestroyDescriptorSetLayout(vk::device, vk::descriptorSetLayout_Light, nullptr);

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		vkDestroyBuffer(vk::device, vk::vertexBuffer[i], nullptr);
		vkFreeMemory(vk::device, vk::vertexBufferMemory[i], nullptr);
	}
	
	vkDestroyBuffer(vk::device, vk::indexBuffer, nullptr);
	vkFreeMemory(vk::device, vk::indexBufferMemory, nullptr);

	vkDestroyDevice(vk::device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(vk::instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(vk::instance, vk::surface, nullptr);
	vkDestroyInstance(vk::instance, nullptr);

}

void vk::createInstance() {
	
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation Layers Requested but Not Available\n");
	}
	
	VkApplicationInfo appInfo{};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan GI";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	auto extensions = getRequiredExtensions();

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &vk::instance);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Vulkan Instance\n");
	}

}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {

	QueueFamilyIndices indices;

	uint32_t queueFamilycount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilycount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilycount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilycount, queueFamilies.data());

	VkBool32 presentSupport = false;

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk::surface, &presentSupport);

		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
			
			if (presentSupport) {
				indices.presentFamily = i;
				break;
			}

		}

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;

}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {

	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk::surface, &details.capablities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk::surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk::surface, &formatCount, details.formats.data());
	}

	uint32_t presentCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk::surface, &presentCount, nullptr);
		
	if (presentCount != 0) {
		details.presentModes.resize(presentCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk::surface, &presentCount, details.presentModes.data());
	}

	return details;

}

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(vk::deviceExtensions.begin(), vk::deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty() && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

}

bool isDeviceSuitable(VkPhysicalDevice device) {

	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.geometryShader;

}

void vk::pickPhysicalDevice() {
	
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vk::instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to Find GPUs with Vulkan Support\n");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vk::instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			vk::physicalDevice = device;
			break;
		}
	}

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(vk::physicalDevice, &deviceProperties);

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to Find a Suitable GPU\n");
	}

}

void vk::createLogicalDevice() {

	QueueFamilyIndices indices = findQueueFamilies(vk::physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.geometryShader = VK_TRUE;
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &vk::device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Logical Device\n");
	}

	vkGetDeviceQueue(vk::device, indices.graphicsFamily.value(), 0, &vk::graphicsQueue);
	vkGetDeviceQueue(vk::device, indices.presentFamily.value(), 0, &vk::presentQueue);

}

void vk::createSurface(GLFWwindow* window) {

	if (glfwCreateWindowSurface(vk::instance, window, nullptr, &vk::surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Window Surface\n");
	}

}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}

	return availableFormats[0];

}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;

}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

}

void vk::createSwapChain(GLFWwindow* window) {

	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vk::physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capablities, window);

	uint32_t imageCount = swapChainSupport.capablities.minImageCount + 1;

	if (swapChainSupport.capablities.maxImageCount > 0 && imageCount > swapChainSupport.capablities.maxImageCount) {
		imageCount = swapChainSupport.capablities.maxImageCount;
	}

	QueueFamilyIndices indices = findQueueFamilies(vk::physicalDevice);
	uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = vk::surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 1;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = swapChainSupport.capablities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(vk::device, &createInfo, nullptr, &vk::swapChain) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Swap Chain\n");
	}

	vkGetSwapchainImagesKHR(vk::device, vk::swapChain, &imageCount, nullptr);
	vk::swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(vk::device, vk::swapChain, &imageCount, swapChainImages.data());
	vk::swapChainImageFormat = surfaceFormat.format;
	vk::swapChainExtent = extent;

}

void vk::createImageViews() {

	swapChainImageViews.resize(swapChainImages.size());
	
	for (size_t i = 0; i < swapChainImages.size(); i++) {

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(vk::device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Image Views\n");
		}

	}

}

static std::vector<char> readFile(const std::string& filename) {
	
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to Open Shader\n");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;

}

VkShaderModule createShaderModule(const std::vector<char>& code) {

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = static_cast<uint32_t>(code.size());
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vk::device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Shader Module\n");
	}

	return shaderModule;

}

VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

	for (auto format : candidates) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(vk::physicalDevice, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
			return format;
		}

	}

	throw std::runtime_error("Cannot Find Supported Format\n");

}

bool hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat findDepthFormat() {

	return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

}

void vk::createRenderPass() {

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = vk::swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachmentDescriptions = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(vk::device, &renderPassInfo, nullptr, &vk::renderPass) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Render Pass\n");
	}

}

//void vk::createDescriptorSetLayout() {
//
//	VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding{};
//	DescriptorSetLayoutBinding.binding = 0;
//	DescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	DescriptorSetLayoutBinding.descriptorCount = 1;
//	DescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//
//	VkDescriptorSetLayoutCreateInfo createInfo{};
//	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//	createInfo.bindingCount = 1;
//	createInfo.pBindings = &DescriptorSetLayoutBinding;
//
//	if (vkCreateDescriptorSetLayout(vk::device, &createInfo, nullptr, &vk::descriptorSetLayout) != VK_SUCCESS) {
//		throw std::runtime_error("Failed to Create Descriptor Layout\n");
//	}
//
//}

void vk::createGraphicsPipeline() {

	vk::graphicsPipeline.resize(vk::NUM_SHADERS);
	vk::pipelineLayout.resize(vk::NUM_SHADERS);

	auto vertexShaderCode = readFile("shaders/vertex.spv");
	auto phongShaderCode = readFile("shaders/Phong.spv");
	auto celShaderCode = readFile("shaders/Cel.spv");
	auto goochShaderCode = readFile("shaders/Gooch.spv");
	auto flatShaderCode = readFile("shaders/Flat.spv");
	auto normalShaderCode = readFile("shaders/Normal.spv");
	auto solidShaderCode = readFile("shaders/Solid.spv");
	auto explodeShaderCode = readFile("shaders/Explode.spv");
	auto wireframeShaderCode = readFile("shaders/Wireframe.spv");

	std::vector<std::vector<char>> fragmentShaderCodes = { phongShaderCode, celShaderCode, goochShaderCode, flatShaderCode, normalShaderCode, solidShaderCode };

	for (size_t i = 0; i < vk::NUM_SHADERS; i++) {
		VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
		VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCodes[i]);
		VkShaderModule geometryShaderModule = createShaderModule(explodeShaderCode);
		if (i == 5) {
			geometryShaderModule = createShaderModule(wireframeShaderCode);
		}

		VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
		vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageInfo.module = vertexShaderModule;
		vertexShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo geometryShaderStageInfo{};
		geometryShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		geometryShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		geometryShaderStageInfo.module = geometryShaderModule;
		geometryShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStageInfo.module = fragmentShaderModule;
		fragmentShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo};

		VkPipelineShaderStageCreateInfo shaderStages_withGeometry[] = { vertexShaderStageInfo, fragmentShaderStageInfo, geometryShaderStageInfo };

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateInfo.pDynamicStates = dynamicStates.data();

		auto bindingDescriptions = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vk::swapChainExtent.width;
		viewport.height = (float)vk::swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = vk::swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.pViewports = &viewport;
		viewportStateInfo.scissorCount = 1;
		viewportStateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizerInfo.depthBiasEnable = VK_FALSE;
		rasterizerInfo.depthBiasConstantFactor = 0.0f;
		rasterizerInfo.depthBiasClamp = 0.0f;
		rasterizerInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachementState{};
		colorBlendAttachementState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachementState.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendingState{};
		colorBlendingState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingState.logicOpEnable = VK_FALSE;
		colorBlendingState.attachmentCount = 1;
		colorBlendingState.pAttachments = &colorBlendAttachementState;

		VkPipelineDepthStencilStateCreateInfo depthStencilState{};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		VkDescriptorSetLayout layouts[] = { vk::descriptorSetLayout_Transformation, vk::descriptorSetLayout_Light };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 2; // static_cast<uint32_t>(layouts.size());
		pipelineLayoutInfo.pSetLayouts = layouts;//&vk::descriptorSetLayout_Transformation;
		//pipelineLayoutInfo.pSetLayouts = &vk::descriptorSetLayout_Light;

		if (vkCreatePipelineLayout(vk::device, &pipelineLayoutInfo, nullptr, &pipelineLayout[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Pipeline Layout\n");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		if (i == 4 || i == 5) {
			pipelineInfo.stageCount = 3;
			pipelineInfo.pStages = shaderStages_withGeometry;
		}
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState = &multisamplingInfo;
		pipelineInfo.pColorBlendState = &colorBlendingState;
		pipelineInfo.pDepthStencilState = &depthStencilState;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = vk::pipelineLayout[i];
		pipelineInfo.renderPass = vk::renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(vk::device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vk::graphicsPipeline[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Graphics Pipeline\n");
		}

		vkDestroyShaderModule(vk::device, vertexShaderModule, nullptr);
		vkDestroyShaderModule(vk::device, fragmentShaderModule, nullptr);
		vkDestroyShaderModule(vk::device, geometryShaderModule, nullptr);
	}

}

void vk::createFramebuffers() {

	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {

		std::array<VkImageView, 2> attachments = {
			swapChainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = vk::renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(vk::device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Framebuffer\n");
		}

	}

}

void vk::createCommandPool() {

	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(vk::physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	//std::cout << "QUEUE FAMILY VALUE HERE - " << queueFamilyIndices.graphicsFamily.value() << "\n";
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(vk::device, &poolInfo, nullptr, &vk::commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Command Pool\n");
	}

}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

	VkPhysicalDeviceMemoryProperties memProp;
	vkGetPhysicalDeviceMemoryProperties(vk::physicalDevice, &memProp);

	for (uint32_t i = 0; i < memProp.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProp.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to Find Sutaible Memory Type\n");

}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
	createInfo.usage = usage;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(vk::device, &createInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Buffer\n");
	}

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(vk::device, buffer, &memReq);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

	if (vkAllocateMemory(vk::device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Allocate Buffer Memory\n");
	}

	vkBindBufferMemory(vk::device, buffer, bufferMemory, 0);

}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vk::commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(vk::device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(vk::graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vk::graphicsQueue);

	vkFreeCommandBuffers(vk::device, vk::commandPool, 1, &commandBuffer);

}

void vk::loadModel() {

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		if (vk::MODEL_PATH[i] == "") {
			return;
		}

		vk::vertices.resize(vk::NUM_MODELS);
		vk::indices.resize(vk::NUM_MODELS);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, vk::MODEL_PATH[i].c_str())) {
			throw std::runtime_error(warn + err);
		}

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertices[i].push_back(vertex);
				indices[i].push_back(indices.size());
			}
		}
	}
}

void vk::createVertexBuffer() {

	vk::vertexBuffer.resize(vk::NUM_MODELS);
	vk::vertexBufferMemory.resize(vk::NUM_MODELS);

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		VkDeviceSize bufferSize = sizeof(vertices[i][0]) * vertices[i].size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(vk::device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices[i].data(), (size_t)bufferSize);
		vkUnmapMemory(vk::device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk::vertexBuffer[i], vk::vertexBufferMemory[i]);
		copyBuffer(stagingBuffer, vk::vertexBuffer[i], bufferSize);

		vkDestroyBuffer(vk::device, stagingBuffer, nullptr);
		vkFreeMemory(vk::device, stagingBufferMemory, nullptr);
	}

}

void vk::createIndexBuffer() {

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(vk::device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(vk::device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk::indexBuffer, vk::indexBufferMemory);
	copyBuffer(stagingBuffer, vk::indexBuffer, bufferSize);

	vkDestroyBuffer(vk::device, stagingBuffer, nullptr);
	vkFreeMemory(vk::device, stagingBufferMemory, nullptr);

}

//void vk::createUniformBuffers(size_t size, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped) {
//
//	VkDeviceSize bufferSize = size;
//
//	uniformBuffers.resize(vk::MAX_FRAMES_IN_FLIGHT);
//	uniformBuffersMemory.resize(vk::MAX_FRAMES_IN_FLIGHT);
//	uniformBuffersMapped.resize(vk::MAX_FRAMES_IN_FLIGHT);
//
//	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
//		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
//		
//		vkMapMemory(vk::device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
//	}
//
//}

void vk::createDescriptorPool() {

	std::array<VkDescriptorPoolSize, 3> poolSize{};
	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = 2*static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT)*static_cast<uint32_t>(vk::NUM_MODELS);
	poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[1].descriptorCount = 2 * static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT)*static_cast<uint32_t>(vk::NUM_MODELS);
	poolSize[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[2].descriptorCount = 2 * static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT)*static_cast<uint32_t>(vk::NUM_MODELS);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
	poolInfo.pPoolSizes = poolSize.data();
	poolInfo.maxSets = 2 * static_cast<uint32_t>(vk::MAX_FRAMES_IN_FLIGHT) * static_cast<uint32_t>(vk::NUM_MODELS);

	if (vkCreateDescriptorPool(vk::device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Descriptor Pool\n");
	}

}

//void vk::createDescriptorSets(std::vector<VkBuffer>& uniformBuffers) {
//
//	std::vector<VkDescriptorSetLayout> layouts(vk::MAX_FRAMES_IN_FLIGHT, vk::descriptorSetLayout);
//	VkDescriptorSetAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//	allocInfo.descriptorPool = vk::descriptorPool;
//	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//	allocInfo.pSetLayouts = layouts.data();
//
//	int size = descriptorSets.size();
//	std::vector<VkDescriptorSet> DS;
//
//	DS.resize(vk::MAX_FRAMES_IN_FLIGHT);
//	if (vkAllocateDescriptorSets(vk::device, &allocInfo, DS.data()) != VK_SUCCESS) {
//		throw std::runtime_error("Failed to Allocate Descriptor Sets\n");
//	}
//
//	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
//		VkDescriptorBufferInfo bufferInfo{};
//		bufferInfo.buffer = uniformBuffers[i];
//		bufferInfo.offset = 0;
//		bufferInfo.range = sizeof(Transformations);
//
//		VkWriteDescriptorSet descriptorWrite{};
//		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		descriptorWrite.dstSet = DS[i];
//		descriptorWrite.dstBinding = 0;
//		descriptorWrite.dstArrayElement = 0;
//		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		descriptorWrite.descriptorCount = 1;
//		descriptorWrite.pBufferInfo = &bufferInfo;
//		descriptorWrite.pImageInfo = nullptr;
//		descriptorWrite.pTexelBufferView = nullptr;
//
//		vkUpdateDescriptorSets(vk::device, 1, &descriptorWrite, 0, nullptr);
//	}
//
//	descriptorSets.push_back(DS);
//
//}

void vk::createDescriptorSetLayout_Transformation() {

	VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding{};
	DescriptorSetLayoutBinding.binding = 0;
	DescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorSetLayoutBinding.descriptorCount = 1;
	DescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding SamplerLayoutBinding{};
	SamplerLayoutBinding.binding = 1;
	SamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	SamplerLayoutBinding.pImmutableSamplers = nullptr;
	SamplerLayoutBinding.descriptorCount = 1;
	SamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { DescriptorSetLayoutBinding, SamplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	createInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(vk::device, &createInfo, nullptr, &vk::descriptorSetLayout_Transformation) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Descriptor Layout\n");
	}

}

void vk::createDescriptorSetLayout_Light() {

	VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding{};
	DescriptorSetLayoutBinding.binding = 1;
	DescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorSetLayoutBinding.descriptorCount = 1;
	DescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = 1;
	createInfo.pBindings = &DescriptorSetLayoutBinding;

	if (vkCreateDescriptorSetLayout(vk::device, &createInfo, nullptr, &vk::descriptorSetLayout_Light) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Create Descriptor Layout\n");
	}

}

void vk::createDescriptor_Transformation() {

	VkDeviceSize bufferSize = sizeof(Transformations);
	
	uniformBuffers_Transformation.resize(vk::MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory_Transformation.resize(vk::MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped_Transformation.resize(vk::MAX_FRAMES_IN_FLIGHT);
	
	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		uniformBuffers_Transformation[i].resize(vk::NUM_MODELS);
		uniformBuffersMemory_Transformation[i].resize(vk::NUM_MODELS);
		uniformBuffersMapped_Transformation[i].resize(vk::NUM_MODELS);
		for (size_t j = 0; j < vk::NUM_MODELS; j++) {
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers_Transformation[i][j], uniformBuffersMemory_Transformation[i][j]);

			vkMapMemory(vk::device, vk::uniformBuffersMemory_Transformation[i][j], 0, bufferSize, 0, &vk::uniformBuffersMapped_Transformation[i][j]);
		}
	}

	std::vector<VkDescriptorSetLayout> layouts(vk::NUM_MODELS, vk::descriptorSetLayout_Transformation);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vk::descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(NUM_MODELS);
	allocInfo.pSetLayouts = layouts.data();
	
	descriptorSets_Transformation.resize(vk::MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		descriptorSets_Transformation[i].resize(vk::NUM_MODELS);
		VkResult result = vkAllocateDescriptorSets(vk::device, &allocInfo, descriptorSets_Transformation[i].data());
		if (result != VK_SUCCESS) {
			std::cout << "RESULT - " << result << "\n";
			throw std::runtime_error("Failed to Allocate Descriptor Sets\n");
		}
	}
	
	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		for (size_t j = 0; j < vk::NUM_MODELS; j++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers_Transformation[i][j];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(Transformations);

			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vk::textureImageView[j];
			imageInfo.sampler = vk::textureSampler[j];

			std::array<VkWriteDescriptorSet, 2> descriptorWrite{};
			descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite[0].dstSet = descriptorSets_Transformation[i][j];
			descriptorWrite[0].dstBinding = 0;
			descriptorWrite[0].dstArrayElement = 0;
			descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite[0].descriptorCount = 1;
			descriptorWrite[0].pBufferInfo = &bufferInfo;
			descriptorWrite[0].pImageInfo = nullptr;
			descriptorWrite[0].pTexelBufferView = nullptr;

			descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite[1].dstSet = descriptorSets_Transformation[i][j];
			descriptorWrite[1].dstBinding = 1;
			descriptorWrite[1].dstArrayElement = 0;
			descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite[1].descriptorCount = 1;
			descriptorWrite[1].pImageInfo = &imageInfo;
			descriptorWrite[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(vk::device, static_cast<uint32_t>(descriptorWrite.size()), descriptorWrite.data(), 0, nullptr);
		}
	}

}

void vk::createDescriptor_Light() {

	VkDeviceSize bufferSize = sizeof(Light);

	uniformBuffers_Light.resize(vk::MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory_Light.resize(vk::MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped_Light.resize(vk::MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		uniformBuffers_Light[i].resize(vk::NUM_MODELS);
		uniformBuffersMemory_Light[i].resize(vk::NUM_MODELS);
		uniformBuffersMapped_Light[i].resize(vk::NUM_MODELS);
		for (size_t j = 0; j < vk::NUM_MODELS; j++) {
			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers_Light[i][j], uniformBuffersMemory_Light[i][j]);

			vkMapMemory(vk::device, vk::uniformBuffersMemory_Light[i][j], 0, bufferSize, 0, &vk::uniformBuffersMapped_Light[i][j]);
		}
	}

	std::vector<VkDescriptorSetLayout> layouts(vk::NUM_MODELS, vk::descriptorSetLayout_Light);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vk::descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(NUM_MODELS);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets_Light.resize(vk::MAX_FRAMES_IN_FLIGHT);
	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		descriptorSets_Light[i].resize(vk::NUM_MODELS);
		if (vkAllocateDescriptorSets(vk::device, &allocInfo, descriptorSets_Light[i].data()) != VK_SUCCESS) {
			std::cout << "ALLOC FAILED\n";
			throw std::runtime_error("Failed to Allocate Descriptor Sets\n");
		}
	}

	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		for (size_t j = 0; j < vk::NUM_MODELS; j++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers_Light[i][j];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(Light);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets_Light[i][j];
			descriptorWrite.dstBinding = 1;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(vk::device, 1, &descriptorWrite, 0, nullptr);
		}
	}

}

void vk::createCommandBuffer() {

	commandBuffer.resize(vk::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vk::commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffer.size());

	if (vkAllocateCommandBuffers(vk::device, &allocInfo, vk::commandBuffer.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Allocate Command Buffers\n");
	}

}

void vk::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame, uint32_t model) {

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Begin Recording Command Buffer\n");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vk::renderPass;
	renderPassInfo.framebuffer = vk::swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = vk::swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};//{ {{0.2f, 0.3f, 0.3f, 1.0f}} };
	clearValues[0].color = { {0.125f, 0.0f, 0.1625f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk::graphicsPipeline[model]);

	//vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(vk::swapChainExtent.width);
	viewport.height = static_cast<float>(vk::swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = vk::swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

}

void vk::drawMesh(VkCommandBuffer commandBuffer, uint32_t currentFrame, uint32_t model, uint32_t shader) {

	VkBuffer vertexBuffers[] = { vk::vertexBuffer[model] };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, vk::indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk::pipelineLayout, 0, 1, &vk::descriptorSets_Light[currentFrame], 0, nullptr);

	std::vector<VkDescriptorSet> descriptorSets = { vk::descriptorSets_Transformation[currentFrame][model], vk::descriptorSets_Light[currentFrame][model] };

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk::pipelineLayout[shader], 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices[model].size()), 1, 0, 0);

}

void vk::endRenderPass(VkCommandBuffer commandBuffer) {

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Record Command Buffer\n");
	}

}

void vk::createSyncObjects() {

	vk::imageAvailableSemaphore.resize(vk::MAX_FRAMES_IN_FLIGHT);
	vk::renderFinishedSemaphore.resize(vk::MAX_FRAMES_IN_FLIGHT);
	vk::inFlightFence.resize(vk::MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(vk::device, &semaphoreInfo, nullptr, &vk::imageAvailableSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(vk::device, &semaphoreInfo, nullptr, &vk::renderFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(vk::device, &fenceInfo, nullptr, &vk::inFlightFence[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Create Sync Objects\n");
		}
	}

}

void vk::cleanupSwapChain() {

	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	for (auto framebuffer : vk::swapChainFramebuffers) {
		vkDestroyFramebuffer(vk::device, framebuffer, nullptr);
	}

	for (auto imageView : vk::swapChainImageViews) {
		vkDestroyImageView(vk::device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, vk::swapChain, nullptr);

}

void vk::recreateSwapChain(GLFWwindow* window) {

	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}


	vkDeviceWaitIdle(vk::device);

	vk::cleanupSwapChain();

	vk::createSwapChain(window);
	vk::createImageViews();
	vk::createDepthResources();
	vk::createFramebuffers();

}

void vk::setModel(std::vector<std::string> path, std::vector<std::string> tex_path) {

	vk::MODEL_PATH.resize(vk::NUM_MODELS);
	vk::TEXTURE_PATH.resize(vk::NUM_MODELS);

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		vk::MODEL_PATH[i] = path[i];
		vk::TEXTURE_PATH[i] = tex_path[i];
	}

}

VkCommandBuffer vk::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = vk::commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer command_Buffer;
	vkAllocateCommandBuffers(vk::device, &allocInfo, &command_Buffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_Buffer, &beginInfo);

	return command_Buffer;
}

void vk::endSingleTimeCommands(VkCommandBuffer command_Buffer) {
	vkEndCommandBuffer(command_Buffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command_Buffer;

	vkQueueSubmit(vk::graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vk::graphicsQueue);

	vkFreeCommandBuffers(vk::device, vk::commandPool, 1, &command_Buffer);
}

void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
	
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(vk::device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(vk::device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(vk::device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(vk::device, image, imageMemory, 0);

}

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(vk::device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;

}

void vk::createDepthResources() {

	VkFormat format = findDepthFormat();

	createImage(vk::swapChainExtent.width, vk::swapChainExtent.height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk::depthImage, vk::depthImageMemory);

	depthImageView = createImageView(vk::depthImage, format, VK_IMAGE_ASPECT_DEPTH_BIT);

}

void transitionImageLayout(VkImage image, VkFormat imageFormat, VkImageLayout oldLayout, VkImageLayout newLayout) {

	VkCommandBuffer commandBuffer = vk::beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::runtime_error("INVALID LAYOUT COMBINATION\n");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	vk::endSingleTimeCommands(commandBuffer);

}

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {

	VkCommandBuffer commandBuffer = vk::beginSingleTimeCommands();

	VkBufferImageCopy copyRegion{};
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;
	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.layerCount = 1;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageOffset = { 0, 0, 0 };
	copyRegion.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

	vk::endSingleTimeCommands(commandBuffer);

}


void vk::createTextureImage() {

	vk::textureImage.resize(vk::NUM_MODELS);
	vk::textureImageMemory.resize(vk::NUM_MODELS);

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(vk::TEXTURE_PATH[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		//std::cout << vk::TEXTURE_PATH[i] << "\n";

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(vk::device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(vk::device, stagingBufferMemory);

		stbi_image_free(pixels);

		createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk::textureImage[i], vk::textureImageMemory[i]);

		transitionImageLayout(vk::textureImage[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(stagingBuffer, vk::textureImage[i], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

		transitionImageLayout(vk::textureImage[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(vk::device, stagingBuffer, nullptr);
		vkFreeMemory(vk::device, stagingBufferMemory, nullptr);
	}
}

void vk::createTextureImageView() {

	vk::textureImageView.resize(vk::NUM_MODELS);

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = vk::textureImage[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.levelCount = 1;

		if (vkCreateImageView(vk::device, &createInfo, nullptr, &vk::textureImageView[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed To create Texture Image view\n");
		}
	}

}

void vk::createTextureSampler() {

	vk::textureSampler.resize(vk::NUM_MODELS);

	for (size_t i = 0; i < vk::NUM_MODELS; i++) {
		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.magFilter = VK_FILTER_LINEAR;
		createInfo.minFilter = VK_FILTER_LINEAR;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(vk::physicalDevice, &properties);
		createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;
		createInfo.compareEnable = VK_FALSE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.mipLodBias = 0.0f;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = 0.0f;

		if (vkCreateSampler(vk::device, &createInfo, nullptr, &vk::textureSampler[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create texture Sampler\n");
		}
	}

}

void vk::setNumModels(int num) {

	vk::NUM_MODELS = num;

}