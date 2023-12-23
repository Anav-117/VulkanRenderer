#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <string>

namespace vk {

	extern VkDevice device;
	extern std::vector<VkFence> inFlightFence;
	extern std::vector<VkSemaphore> imageAvailableSemaphore;
	extern std::vector<VkSemaphore> renderFinishedSemaphore;
	extern VkSwapchainKHR swapChain;
	extern std::vector<VkCommandBuffer> commandBuffer;
	extern std::vector<VkBuffer> vertexBuffer;
	extern std::vector<VkDeviceMemory> vertexBufferMemory;
	extern VkBuffer indexBuffer;
	extern VkDeviceMemory indexBufferMemory;
	extern VkExtent2D swapChainExtent;

	extern bool FramebufferResized;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	extern VkQueue presentQueue;
	extern VkQueue graphicsQueue;

	extern std::vector<std::vector<VkBuffer>> uniformBuffers_Transformation;
	extern std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory_Transformation;
	extern std::vector<std::vector<void*>> uniformBuffersMapped_Transformation;

	extern std::vector<std::vector<VkBuffer>> uniformBuffers_Light;
	extern std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory_Light;
	extern std::vector<std::vector<void*>> uniformBuffersMapped_Light;

	void Init(GLFWwindow* window);
	void Cleanup();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame, uint32_t model);
	void recreateSwapChain(GLFWwindow* window);
	void setNumModels(int num);
	void setModel(std::vector<std::string> path, std::vector<std::string> tex_path);
	void ImGuiInit(GLFWwindow* window);
	void drawMesh(VkCommandBuffer commandBuffer, uint32_t currentFrame, uint32_t model, uint32_t shader);
	void endRenderPass(VkCommandBuffer commandBuffer);

}

struct Transformations {

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	float time;

};

struct Light {

	glm::vec3 direction;
	glm::vec3 viewPos;

};