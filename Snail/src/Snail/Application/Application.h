#pragma once

#include "Snail/Core.h"

namespace Snail {

	class SNAIL_API Application
	{
	public:
		Application();
		// Application 类会被Example所继承，所以使用virtual
		virtual ~Application();

		void run();
	};

	// 将在客户端（子类中）给出函数体
	Application* CreateApplication();

}

