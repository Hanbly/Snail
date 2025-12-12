#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

namespace Snail {

	struct FrameBufferSpecification {
		uint32_t width, height;
		uint32_t samples = 1;

		bool SwapChainTarget = false;

		FrameBufferSpecification(const uint32_t& width, const uint32_t& height)
			: width(width), height(height)
		{
		}
	};

	class FrameBuffer {
	public:
		virtual ~FrameBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual const uint32_t& GetColorAttachment() const = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;

		virtual void ReGenerate() = 0;
		virtual void Resize(const uint32_t& width, const uint32_t& height) = 0;
		virtual void Delete() = 0;

		static Refptr<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};

}