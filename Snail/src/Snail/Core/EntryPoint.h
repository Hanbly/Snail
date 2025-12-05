#pragma once

#ifdef SNL_PLATFORM_WINDOWS

	extern Snail::Application* Snail::CreateApplication();

	int main(int argc, char** argv)
	{
		SetConsoleOutputCP(CP_UTF8);

		Snail::Log::InitLoggers();

		Snail::SNL_PROFILE_BEGIN_SESSION("Setup", "SnailProfile-Setup.json");
		auto app = Snail::CreateApplication();
		Snail::SNL_PROFILE_END_SESSION();

		Snail::SNL_PROFILE_BEGIN_SESSION("Runtime", "SnailProfile-Runtime.json");
		app->run();
		Snail::SNL_PROFILE_END_SESSION();

		Snail::SNL_PROFILE_BEGIN_SESSION("Shutdown", "SnailProfile-Shutdown.json");
		delete app;
		Snail::SNL_PROFILE_END_SESSION();

		return 0;
	}

#endif