#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

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