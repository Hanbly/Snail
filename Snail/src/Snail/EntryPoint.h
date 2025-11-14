#pragma once

#ifdef SNL_PLATFORM_WINDOWS

	extern Snail::Application* Snail::CreateApplication();

	int main(int argc, char** argv)
	{
		Snail::Log::InitLoggers();
		SNL_CORE_TRACE("CoreLogger Initialize!");
		SNL_ERROR("and ClientLogger");

		auto app = Snail::CreateApplication();
		app->run();
		delete app;
	}

#endif
