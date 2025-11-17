#pragma once

#ifdef SNL_PLATFORM_WINDOWS

#include <windows.h>

	extern Snail::Application* Snail::CreateApplication();

	int main(int argc, char** argv)
	{
		SetConsoleOutputCP(CP_UTF8);

		Snail::Log::InitLoggers();
		SNL_CORE_TRACE("核心日志初始化!");
		SNL_ERROR("客户端日志初始化错误");

		auto app = Snail::CreateApplication();
		app->run();
		delete app;
	}

#endif
