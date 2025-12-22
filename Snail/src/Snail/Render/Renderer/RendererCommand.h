#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"

#include <glm/glm.hpp>

namespace Snail {

	class SNAIL_API RendererCommand
	{
	public:
		enum class API {
			None = 0,
			OpenGL,
			Vulkan
		};
		enum class WindowAPI {
			None = 0,
			GLFW
		};
		enum class StencilFuncType {
			None = 0,
			ALWAYS,
			NOTEQUAL
		};
		enum class DepthFuncType {
			None = 0,
			LESS,
			LEQUAL,
			ALWAYS
		};
		virtual ~RendererCommand() = default;

		inline static API GetAPI() { return s_API; }
		inline static WindowAPI GetWindowAPI() { return s_WindowAPI; }
	private:
		static API s_API;
		static WindowAPI s_WindowAPI;
	private: //-------------------------Commands-------------------------------------
		static Uniptr<RendererCommand> RC;

		virtual void InitImpl() = 0;
		virtual void SetViewPortImpl(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void ClearColorImpl(const glm::vec4& color_RGBA) const = 0;
		virtual void ClearImpl() const = 0;
		virtual void DrawIndexedImpl(const Refptr<VertexArray>& vertexArray) const = 0;

		virtual void DrawIndexedInstancedImpl(const Refptr<VertexArray>& vertexArray, uint32_t indexCount) const = 0;

		virtual void StencilFuncImpl(const StencilFuncType& type, const int& ref, const int& mask) const = 0;
		virtual void StencilMaskImpl(const bool& status) const = 0;
		virtual void DepthTestImpl(const bool& enable) const = 0;
		virtual void SetDepthFuncImpl(const DepthFuncType& type) const = 0;
		virtual void EnableBlendImpl(const bool& enable) const = 0;

	public:
		inline static void Init() 
		{ RC->InitImpl(); }
		inline static void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) 
		{ RC->SetViewPortImpl(x, y, width, height); }

		inline static void ClearColor(const glm::vec4& color_RGBA) 
		{ RC->ClearColorImpl(color_RGBA); }
		inline static void Clear() 
		{ RC->ClearImpl(); }
		inline static void DrawIndexed(const Refptr<VertexArray>& vertexArray) 
		{ RC->DrawIndexedImpl(vertexArray); }
		inline static void DrawIndexedInstanced(const Refptr<VertexArray>& vertexArray, uint32_t indexCount)
		{ RC->DrawIndexedInstancedImpl(vertexArray, indexCount); }

		inline static void StencilFunc(const StencilFuncType& type, const int& ref, const int& mask)
		{ RC->StencilFuncImpl(type, ref, mask); }
		inline static void StencilMask(const bool& status) 
		{ RC->StencilMaskImpl(status); }
		inline static void DepthTest(const bool& enable) 
		{ RC->DepthTestImpl(enable); }
		inline static void SetDepthFunc(const DepthFuncType& type) 
		{ RC->SetDepthFuncImpl(type); }
		inline static void EnableBlend(const bool& enable)
		{ RC->EnableBlendImpl(enable); }

		static Uniptr<RendererCommand> Create();
	};

}
