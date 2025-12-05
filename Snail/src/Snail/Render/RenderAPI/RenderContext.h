#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

namespace Snail {

	class SNAIL_API RenderContext {
	protected:
		virtual void InitContextImpl() = 0;
		virtual void SwapBuffersImpl() = 0;
	public:
		virtual ~RenderContext() {}

		inline void InitContext() { this->InitContextImpl(); }
		inline void SwapBuffers() { this->SwapBuffersImpl(); }

		static Uniptr<RenderContext> Create(void* windowHandle);
	};

}