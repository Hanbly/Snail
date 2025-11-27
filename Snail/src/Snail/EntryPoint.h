#pragma once

#ifdef SNL_PLATFORM_WINDOWS

	extern Snail::Application* Snail::CreateApplication();

	int main(int argc, char** argv)
	{
		SetConsoleOutputCP(CP_UTF8);

		Snail::Log::InitLoggers();

		auto app = Snail::CreateApplication();
		app->run();
		delete app;

		return 0;
	}

#endif