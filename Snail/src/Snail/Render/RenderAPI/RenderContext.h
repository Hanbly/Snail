#pragma once

#include "Snail/Core.h"

namespace Snail {

	class SNAIL_API RenderContext {
	protected:
		virtual void InitContextImpl() = 0;
		virtual void SwapBuffersImpl() = 0;
	public:
		inline void InitContext() { this->InitContextImpl(); }
		inline void SwapBuffers() { this->SwapBuffersImpl(); }

		static std::unique_ptr<RenderContext> CreateRenderContext(void* windowHandle);
	};

}