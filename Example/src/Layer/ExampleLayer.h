#pragma once

#include "Snail\Layer\Layer.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Snail/Render/RenderAPI/Buffer/VertexBuffer.h"
#include "Snail/Render/RenderAPI/Buffer/IndexBuffer.h"
#include "Snail/Render/RenderAPI/Shader.h"
#include "Snail/Render/Renderer/Renderer.h"
#include "Snail/Render/Renderer/Camera/Camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <glad/glad.h>
#include "GLFW/glfw3.h"

class ExampleLayer : public Snail::Layer
{
private:
	// -------------------临时------------------------------------------
	std::shared_ptr<Snail::VertexArray> m_VertexArray;
	std::shared_ptr<Snail::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<Snail::IndexBuffer> m_IndexBuffer;
	std::shared_ptr<Snail::Shader> m_Shader;
	std::unique_ptr<Snail::Camera> m_Camera;
public:
	ExampleLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	virtual void OnAttach() override {
		// -------------------临时------------------------------------------
		m_VertexArray = Snail::VertexArray::CreateVertexArray();
		m_VertexArray->Bind();

		float vertices[] = {
			// 前面 4 个点 (Z = 0.5)
			-0.5f, -0.5f,  0.5f, // 0: 左下前
			 0.5f, -0.5f,  0.5f, // 1: 右下前
			 0.5f,  0.5f,  0.5f, // 2: 右上前
			-0.5f,  0.5f,  0.5f, // 3: 左上前

			// 后面 4 个点 (Z = -0.5)
			-0.5f, -0.5f, -0.5f, // 4: 左下后
			 0.5f, -0.5f, -0.5f, // 5: 右下后
			 0.5f,  0.5f, -0.5f, // 6: 右上后
			-0.5f,  0.5f, -0.5f  // 7: 左上后
		};
		m_VertexBuffer = Snail::VertexBuffer::CreateVertexBuffer(vertices, sizeof(vertices));
		m_VertexBuffer->Bind();
		// 创建 & 启用布局layout
		std::shared_ptr<Snail::BufferLayout> layout = Snail::BufferLayout::CreateBufferLayout(
			{
				{ "positions", Snail::VertexDataType::Float3 },
				{ "texture_coords", Snail::VertexDataType::Float2 }
			}
		);
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[] = {
			// 前面
			0, 1, 2,
			2, 3, 0,
			// 右面
			1, 5, 6,
			6, 2, 1,
			// 后面
			7, 6, 5,
			5, 4, 7,
			// 左面
			4, 0, 3,
			3, 7, 4,
			// 上面
			3, 2, 6,
			6, 7, 3,
			// 下面
			4, 5, 1,
			1, 0, 4
		};
		m_IndexBuffer = Snail::IndexBuffer::CreateIndexBuffer(indices, sizeof(indices));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);


		m_Shader = Snail::Shader::CreateShader("shaders/test.shader");

		m_Camera = std::make_unique<Snail::Camera>(45.0f, 1920.0f/1080.0f, glm::vec3(0.0f, 0.0f, 3.0f));
		//------------------------------------------------------------------------------
	}
	virtual void OnDetach() override {

	}

	inline void OnUpdate() override {
		SNL_TRACE("ExampleLayer 调用: OnUpdate()");

		// -------------------临时------------------------------------------
		Snail::Renderer::BeginScene(m_Camera);

		// 2. 创建变换矩阵
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// 3. 计算变换 (现在有了头文件，这些函数就能用了)
		model = glm::rotate(model, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		// --- 修改开始：让物体动起来 ---
		// 获取运行时间（秒）
		float timeValue = (float)glfwGetTime();
		// 旋转逻辑：
		// 1. 每一帧都在 X 轴上稍微倾斜一点 (-55度常数，模拟俯视)
		// 2. 每一帧都绕 Z 轴持续旋转 (timeValue * 50度/秒)
		// 注意：旋转顺序很重要（先自转，再倾斜，或者反过来）
		// 这里我们让它绕着一条斜轴 (1.0, 0.5, 0.0) 持续旋转，这样看得最清楚
		model = glm::rotate(model, glm::radians(timeValue * 50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		// 观察位置：稍微离远一点，看全貌
		/*view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));*/
		m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f + timeValue));
		// --- 修改结束 ---
		// 
		// 4. 设置 Uniforms 到 Shader
		// --- 设置 model 矩阵 ---
		m_Shader->SetUniformMatrix4fv("model", model);

		
		// 5. 渲染
		// --- 设置 view 矩阵 ---
		// --- 设置 projection 矩阵 --- 也在submit实现
		Snail::Renderer::Submit(m_Shader, m_VertexArray);


		Snail::Renderer::EndScene();
		//----------------------------------------------------------------
	}

	inline void OnEvent(Snail::Event& e) {
		//SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
	}

	inline void OnRender() override {
		//SNL_TRACE("ExampleLayer 调用: OnRender()");
	}
};

