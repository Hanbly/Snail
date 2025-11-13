#pragma once

#ifdef SNL_PLATFORM_WINDOWS

	extern Snail::Application* Snail::CreateApplication();

	int main(int argc, char** argv)
	{
		auto app = Snail::CreateApplication();
		app->run();
		delete app;
	}

#endif
