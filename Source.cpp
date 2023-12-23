#include "VulkanConfig.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include <irrklang/irrKlang.h>

namespace window {
	uint32_t WIDTH = 1920;
	uint32_t HEIGHT = 1080;
}

namespace sound {
	irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
}

namespace camera {
	glm::vec3 pos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 fwd = glm::vec3(0.0f, 0.0f, -1.0f);
	float lastX = window::WIDTH / 2;
	float lastY = window::HEIGHT/ 2;
	bool firstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float fov = 45.0f;
}

namespace model {
	std::vector<std::string> MODEL_PATH;// = { "models/Cereal.obj", "models/Cheese.obj", "models/BigCheese.obj", "models/Brioche.obj", "models/BreadRoll.obj",
										//	"models/Cake.obj" , "models/Cupcake.obj" , "models/Gourd.obj" , "models/Meat.obj" , "models/Loaf.obj", "models/Orange.obj", 
										//	"models/Pepper.obj", "models/Potato.obj", "models/RPotato.obj", "models/Sandwich.obj", "models/bread-1.obj", 
										//	"models/bread-2.obj" , "models/bread-3.obj" , "models/bread-4.obj", "models/bread-roll-1.obj", "models/bread-roll-2.obj",
										//	"models/bread-roll-3.obj" , "models/bread-roll-4.obj", "models/pastry-1.obj", "models/pastry-2.obj", "models/pastry-3.obj",
										//	"models/pastry-4.obj", "models/Rishi.obj" };
	std::vector<std::string> TEXTURE_PATH;	// = { "models/Cereal.jpg", "models/Cheese.jpg", "models/BigCheese.jpg", "models/Brioche.jpg", "models/BreadRoll.jpg",
											//	"models/Cake.jpg" , "models/Cupcake.jpg" , "models/Gourd.jpg" , "models/Meat.jpg" , "models/Loaf.jpg", "models/Orange.jpg", 
											//	"models/Pepper.jpg", "models/Potato.jpg", "models/RPotato.jpg", "models/Sandwich.jpg", "models/bread-1.jpg",
											//	"models/bread-2.jpg" , "models/bread-3.jpg" , "models/bread-4.jpg", "models/bread-roll-1.jpg", "models/bread-roll-2.jpg",
											//	"models/bread-roll-3.jpg" , "models/bread-roll-4.jpg", "models/pastry-1.jpg", "models/pastry-2.jpg", "models/pastry-3.jpg",
											//	"models/pastry-4.jpg", "models/Rishi.jpg" };
	int numModels = 0;// MODEL_PATH.size();
	std::vector<float> X;
	std::vector<float> Y;
	std::vector<float> Z;
	float angle_x = 0;
	float angle_y = 0.5;
	float angle_z = 0;
	int shader = 0;
	int currentModel;
	float scale = 1.0f;
	bool partyMode = false;
}

namespace light {
	glm::vec3 dir = glm::vec3(0.0f, 1.0f, 1.0f);
}

namespace scene {
	bool GI = false;
}

uint32_t currentFrame = 0;

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float currentFrame = glfwGetTime();
	deltaTime = 0.1f;// currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_W) {
		camera::pos = camera::pos + glm::vec3(0.0f, 0.0f, 0.1f) * deltaTime;
	}
	if (key == GLFW_KEY_S) {
		camera::pos = camera::pos + glm::vec3(0.0f, 0.0f, -0.1f) * deltaTime;
	}
	if (key == GLFW_KEY_A) {
		camera::pos = camera::pos + glm::vec3(-0.1f, 0.0f, 0.0f) * deltaTime;
	}
	if (key == GLFW_KEY_D) {
		camera::pos = camera::pos + glm::vec3(0.1f, 0.0f, 0.0f) * deltaTime;

	}
	if (key == GLFW_KEY_Q) {
		camera::pos = camera::pos + glm::vec3(0.0f, 0.1f, 0.0f) * deltaTime;
	}
	if (key == GLFW_KEY_E) {
		camera::pos = camera::pos + glm::vec3(0.0f, -0.1f, 0.0f) * deltaTime;
	}
	if (key == GLFW_KEY_I) {
		light::dir = light::dir + glm::vec3(0.0f, 0.1f, 0.0f);
	}
	if (key == GLFW_KEY_K) {
		light::dir = light::dir + glm::vec3(0.0f, -0.1f, 0.0f);
	}
	if (key == GLFW_KEY_J) {
		light::dir = light::dir + glm::vec3(-0.1f, 0.0f, 0.0f);
	}
	if (key == GLFW_KEY_L) {
		light::dir = light::dir + glm::vec3(0.1f, 0.0f, 0.0f);

	}
	if (key == GLFW_KEY_U) {
		light::dir = light::dir + glm::vec3(0.0f, 0.0f, 0.1f);
	}
	if (key == GLFW_KEY_O) {
		light::dir = light::dir + glm::vec3(0.0f, 0.0f, -0.1f);
	}
	if (key == GLFW_KEY_LEFT) {
		model::angle_y -= 0.1f;
	}
	if (key == GLFW_KEY_RIGHT) {
		model::angle_y += 0.1f;
	}
	if (key == GLFW_KEY_UP) {
		model::angle_x -= 0.1f;
	}
	if (key == GLFW_KEY_DOWN) {
		model::angle_x += 0.1f;
	}
	if (key == GLFW_KEY_PERIOD) {
		model::angle_z -= 0.1f;
	}
	if (key == GLFW_KEY_COMMA) {
		model::angle_z += 0.1f;
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		model::shader = (model::shader + 1) % 6;
		sound::SoundEngine->play2D("audio/powerup.wav");
	}
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		model::currentModel = (model::currentModel + 1) % model::numModels;
		model::scale = 1.0f;
		sound::SoundEngine->play2D("audio/bleep.mp3");
	}
	if (key == GLFW_KEY_Z) {
		model::scale -= 0.1f;
	}
	if (key == GLFW_KEY_X) {
		model::scale += 0.1f;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		model::partyMode = !model::partyMode;
		if (!model::partyMode) {
			/*if (sound::SoundEngine->isCurrentlyPlaying("audio/Threshold.wav")) {
				sound::SoundEngine->stopAllSounds();
			}*/
			camera::pos = glm::vec3(0.0, 0.0, 5.0);
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (camera::firstMouse)
	{
		camera::lastX = xpos;
		camera::lastY = ypos;
		camera::firstMouse = false;
	}

	float xoffset = xpos - camera::lastX;
	float yoffset = camera::lastY - ypos;
	camera::lastX = xpos;
	camera::lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera::yaw += xoffset;
	camera::pitch += yoffset;

	if (camera::pitch > 89.0f)
		camera::pitch = 89.0f;
	if (camera::pitch < -89.0f)
		camera::pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(camera::yaw)) * cos(glm::radians(camera::pitch));
	direction.y = sin(glm::radians(camera::pitch));
	direction.z = sin(glm::radians(camera::yaw)) * cos(glm::radians(camera::pitch));
	camera::fwd = glm::normalize(direction);

}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera::fov -= (float)yoffset;
	if (camera::fov < 1.0f)
		camera::fov = 1.0f;
	if (camera::fov > 45.0f)
		camera::fov = 45.0f;
}

void framebufferResizeCallback(GLFWwindow* window, int width, int height) {

	vk::FramebufferResized = true;

}

void idle() {

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

}

void drawOneMesh(uint32_t imageIndex) {

	Light l{};
	Transformations t{};

	vk::recordCommandBuffer(vk::commandBuffer[currentFrame], imageIndex, currentFrame, model::shader);

	t.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), model::angle_z, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), model::angle_x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), model::angle_y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(model::scale)); // time * glm::radians(90.0f)
	t.view = glm::lookAt(camera::pos, camera::pos + camera::fwd, glm::vec3(0.0f, 1.0f, 0.0f));
	t.projection = glm::perspective(glm::radians(45.0f), vk::swapChainExtent.width / (float)vk::swapChainExtent.height, 0.1f, 100.0f);
	t.projection[1][1] *= -1;
	l.direction = light::dir;
	l.viewPos = camera::pos;

	memcpy(vk::uniformBuffersMapped_Light[currentFrame][model::currentModel], &l, sizeof(l));
	memcpy(vk::uniformBuffersMapped_Transformation[currentFrame][model::currentModel], &t, sizeof(t));

	vk::drawMesh(vk::commandBuffer[currentFrame], currentFrame, model::currentModel, model::shader);

	//if (model::currentModel == model::numModels - 1) {
	//	//sound::SoundEngine->play2D("audio/Halo Theme Song Original.ogg", true);
	//}
	//else {
	//	if (sound::SoundEngine->isCurrentlyPlaying("audio/Halo Theme Song Original.ogg")) {
	//		sound::SoundEngine->stopAllSounds();
	//	}
	//}

	vk::endRenderPass(vk::commandBuffer[currentFrame]);

}

void drawParty(uint32_t imageIndex) {

	//sound::SoundEngine->play2D("audio/Threshold.wav");

	Light l{};
	Transformations t{};

	camera::pos = glm::vec3(3.0f, 4.0f, 12.0f);

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	vk::recordCommandBuffer(vk::commandBuffer[currentFrame], imageIndex, currentFrame, model::shader);

	for (int i = 0; i < model::numModels; i++) {

		t.model = glm::translate(glm::mat4(1.0f), glm::vec3(i%5*1.5, i/5*1.5, 0)) * glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(model::X[i], model::Y[i], model::Z[i])); // time * glm::radians(90.0f)
		t.view = glm::lookAt(camera::pos, camera::pos + camera::fwd, glm::vec3(0.0f, 1.0f, 0.0f));
		t.projection = glm::perspective(glm::radians(45.0f), vk::swapChainExtent.width / (float)vk::swapChainExtent.height, 0.1f, 100.0f);
		t.projection[1][1] *= -1;
		t.time = glfwGetTime();
		l.direction = light::dir;
		l.viewPos = camera::pos;

		memcpy(vk::uniformBuffersMapped_Light[currentFrame][i], &l, sizeof(l));
		memcpy(vk::uniformBuffersMapped_Transformation[currentFrame][i], &t, sizeof(t));

		vk::drawMesh(vk::commandBuffer[currentFrame], currentFrame, i, model::shader);

	}

	vk::endRenderPass(vk::commandBuffer[currentFrame]);

}

void directIllumination(GLFWwindow* window, bool clear) {

	vkWaitForFences(vk::device, 1, &vk::inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(vk::device, vk::swapChain, UINT64_MAX, vk::imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || vk::FramebufferResized) {
		vk::FramebufferResized = false;
		vk::recreateSwapChain(window);
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to Acquire Swap Chain Image\n");
	}

	vkResetFences(vk::device, 1, &vk::inFlightFence[currentFrame]);

	vkResetCommandBuffer(vk::commandBuffer[currentFrame], 0);

	if (model::partyMode) {
		drawParty(imageIndex);
	}
	else {
		drawOneMesh(imageIndex);
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { vk::imageAvailableSemaphore[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vk::commandBuffer[currentFrame];

	VkSemaphore signalSemaphores[] = { vk::renderFinishedSemaphore[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vk::graphicsQueue, 1, &submitInfo, vk::inFlightFence[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to Submit Draw Command Buffer\n");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { vk::swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	vkQueuePresentKHR(vk::presentQueue, &presentInfo);

	currentFrame = (currentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;

}

void display(GLFWwindow* window) {

	if (!scene::GI) {
		//t.model = glm::rotate(glm::mat4(1.0f), model::angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.1, 1.1, 1.1)); // time * glm::radians(90.0f)
		//t.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//t.projection = glm::perspective(glm::radians(45.0f), vk::swapChainExtent.width / (float)vk::swapChainExtent.height, 0.1f, 100.0f);
		//t.projection[1][1] *= -1;
		//l.direction = light::dir;
		//l.outline = 0;

		//memcpy(vk::uniformBuffersMapped_Light[currentFrame], &l, sizeof(l));
		//memcpy(vk::uniformBuffersMapped_Transformation[currentFrame], &t, sizeof(t));
		//directIllumination(window, true);
		
		directIllumination(window, false);
	}

}

int main() {

	glfwInit();

	GLFWwindow* window;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#else
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#endif

	window = glfwCreateWindow(window::WIDTH, window::HEIGHT, "Vulkan Renderer", glfwGetPrimaryMonitor(), nullptr); //glfwGetPrimaryMonitor()

	glfwSetKeyCallback(window, keyboardCallback);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glfwSetCursorPos(window, window::WIDTH/2, window::HEIGHT/2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


	std::ifstream model_file("models.txt");
	if (!model_file) {
		throw std::runtime_error("No Model File Found\n");
	}
	std::ifstream tex_file("textures.txt");
	if (!tex_file) {
		throw std::runtime_error("No Texture File Found\n");
	}

	std::string line;
	while (std::getline(model_file, line)) // Read next line to `line`, stop if no more lines.
	{
		model::MODEL_PATH.push_back(line);
	}

	while (std::getline(tex_file, line)) // Read next line to `line`, stop if no more lines.
	{
		model::TEXTURE_PATH.push_back(line);
	}

	model::numModels = model::MODEL_PATH.size();

	vk::setNumModels(model::numModels);	
	vk::setModel(model::MODEL_PATH, model::TEXTURE_PATH);
	vk::Init(window);

	model::X.resize(model::numModels);
	model::Y.resize(model::numModels);
	model::Z.resize(model::numModels);

	for (int i = 0; i < model::numModels; i++) {
		model::X[i] = (1.0f / static_cast<float>(rand() % 10)) * 2.0f - 1.0f;
		model::Y[i] = (1.0f / static_cast<float>(rand() % 10)) * 2.0f - 1.0f;
		model::Z[i] = (1.0f / static_cast<float>(rand() % 10)) * 2.0f - 1.0f;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		idle();
		display(window);
	}

	vkDeviceWaitIdle(vk::device);

	vk::Cleanup();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;

}