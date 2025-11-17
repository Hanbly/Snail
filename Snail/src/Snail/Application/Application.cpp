#include "SNLpch.h"

#include "Application.h"
#include "Snail/Logger/Log.h"
#include "Snail/Events/ApplicationEvent.h"
#include "Snail/Events/MouseEvent.h"

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