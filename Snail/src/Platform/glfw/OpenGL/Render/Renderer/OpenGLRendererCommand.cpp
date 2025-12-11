#include "SNLpch.h"

#include "OpenGLRendererCommand.h"

namespace Snail {

	void OpenGLRendererCommand::InitImpl()
	{
		// 深度测试
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// 模板测试
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0x00); // 默认先禁止写入模板，防止地面等背景干扰

		// 开启混合
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererCommand::SetViewPortImpl(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererCommand::ClearColorImpl(const glm::vec4& color_RGBA) const
	{
		glClearColor(color_RGBA.r, color_RGBA.g, color_RGBA.b, color_RGBA.a);
	}

	void OpenGLRendererCommand::ClearImpl() const
	{
		glStencilMask(0xFF);
		//         颜色缓冲区               深度测试缓冲区           模板测试缓冲区
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererCommand::DrawIndexedImpl(const Refptr<VertexArray>& vertexArray) const
	{
		const uint32_t& count = vertexArray->GetIndexBuffer()->GetIndexBufferCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererCommand::StencilFuncImpl(const StencilFuncType& type, const int& ref, const int& mask) const
	{
		switch (type) {
			case StencilFuncType::ALWAYS:
				// 设置模板规则：总是通过测试，且写入的数据是 1
				glStencilFunc(GL_ALWAYS, ref, mask);
				break;
			case StencilFuncType::NOTEQUAL:
				// 去除中心
				glStencilFunc(GL_NOTEQUAL, ref, mask);
		}
	}

	void OpenGLRendererCommand::StencilMaskImpl(const bool& status) const
	{
		if (status) {
			glStencilMask(0xFF); // 允许写入
		}
		else {
			glStencilMask(0x00); // 禁止写入
		}
	}

	void OpenGLRendererCommand::DepthTestImpl(const bool& enable) const
	{
		if (enable) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}

}