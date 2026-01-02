#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

namespace Snail {

	enum class FrameBufferColorFormat {
		None = 0,
		RGB8, RGBA8, RGBA16F, RGBA32F
	};

	struct FrameBufferSpecification {
		uint32_t width, height;
		uint32_t samples = 1;
		FrameBufferColorFormat colorFormat = FrameBufferColorFormat::RGB8;

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
		virtual const uint32_t& GetMaskAttachment() const = 0;
		virtual const uint32_t& GetDepthAttachment() const = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;

		virtual void ReGenerate() = 0;
		virtual void Resize(const uint32_t& width, const uint32_t& height) = 0;
		virtual void Delete() = 0;

		// 接收立方体贴图的一个面，用作下一次的绘制对象
		virtual void SetupTextureCubei(const int index, const uint32_t& rendererId) = 0;

		static Refptr<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};

}