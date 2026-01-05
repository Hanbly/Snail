#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

namespace Snail {

	enum class FrameBufferTextureFormat {
		None,
		R32I, RGB8, RGBA8, RGBA16F, RGBA32F,
		DEPTH_COMPONENT, DEPTH24_STENCIL8
	};

	struct FrameBufferAttachmentSpecification {
		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
		// 可以在这里添加 wrap/filter 选项，目前保持简单

		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(FrameBufferTextureFormat format) : TextureFormat(format) {}
	};

	struct FrameBufferSpecification {
		uint32_t width, height;
		std::vector<FrameBufferAttachmentSpecification> attachments;

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

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;
		virtual const FrameBufferSpecification& GetSpecification() const = 0;

		virtual void ReGenerate() = 0;
		virtual void Resize(const uint32_t& width, const uint32_t& height) = 0;
		virtual void Delete() = 0;

		// 接收立方体贴图的一个面，用作下一次的绘制对象
		virtual void SetupTextureCubei(const int index, const uint32_t& rendererId) = 0;

		static Refptr<FrameBuffer> Create(const FrameBufferSpecification& spec);
	};

}