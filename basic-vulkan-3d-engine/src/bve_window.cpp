#include "bve_window.hpp"
#include <stdexcept>

namespace bve {
	BveWindow::BveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
		initWindow();
	}

	BveWindow::~BveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	
	void BveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height,windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}
	void BveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void BveWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto bveWindow = reinterpret_cast<BveWindow*>(glfwGetWindowUserPointer(window));
		bveWindow->framebufferResized = true;
		bveWindow->width = width;
		bveWindow->height = height;
	}

} //namespace bve