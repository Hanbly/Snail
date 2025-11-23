#include "SNLpch.h"

#include "WindowsInput.h"

#include "Snail/Application/Application.h"

namespace Snail {

	Input* Input::s_Instance = new WindowsInput();

	std::pair<int, int> WindowsInput::GetWindowSizeImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		return { width, height };
	}

	std::pair<int, int> WindowsInput::GetWindowPosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		int xpos, ypos;
		glfwGetWindowPos(window, &xpos, &ypos);
		return { xpos, ypos };
	}

	bool WindowsInput::IsMouseButtonImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		auto status = glfwGetMouseButton(window, button);
		return (status == GLFW_PRESS);
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto [xpos, ypos] = GetMousePosImpl();
		return xpos;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto [xpos, ypos] = GetMousePosImpl();
		return ypos;
	}

	std::pair<float, float> WindowsInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		double tempx, tempy;
		glfwGetCursorPos(window, &tempx, &tempy);
		return { static_cast<float>(tempx), static_cast<float>(tempy) };
	}

	bool WindowsInput::IsKeyPressedImpl(unsigned int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetWindow());
		auto status = glfwGetKey(window, static_cast<int>(keycode));
		return (status == GLFW_PRESS) || (status == GLFW_REPEAT);
	}

}

