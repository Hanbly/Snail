#include "Application.h"

#include "Log.h"
#include "Events/ApplicationEvent.h"

namespace Snail {

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::run()
	{
		WindowResizeEvent winRE = WindowResizeEvent(1000, 750);
		if (winRE.IsEventInCategory(InputCategoryEvent)) {
			SNL_ERROR("");
		}
		SNL_TRACE(winRE.ToString());

		while (true);
	}

}