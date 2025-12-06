#include "SNLpch.h"

#include "Renderer.h"
#include "Renderer3D.h"

namespace Snail {

	void Renderer::Init()
	{
		// 初始化opengl等基础设置
		RendererCommand::Init();
		// 初始化Renderer3D
		Renderer3D::Init();
	}

	void Renderer::SetViewPort(uint32_t width, uint32_t height)
	{
		// TODO: 以后不一定是从左下角0，0开始设置视口
		// 视口可能还需要更多其它操作
		RendererCommand::SetViewPort(0, 0, width, height);
	}

}