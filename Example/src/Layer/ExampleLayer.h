#pragma once

#include "Snail.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ExampleLayer : public Snail::Layer
{
private:
	// -------------------临时------------------------------------------
	std::shared_ptr<Snail::VertexArray> m_VertexArray;
	std::shared_ptr<Snail::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<Snail::IndexBuffer> m_IndexBuffer;
	std::shared_ptr<Snail::Shader> m_Shader;
	std::unique_ptr<Snail::PerspectiveCameraController> m_CameraController;
public:
	ExampleLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	virtual void OnAttach() override {
		// -------------------临时------------------------------------------
		m_VertexArray = Snail::VertexArray::Create();
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
		m_VertexBuffer = Snail::VertexBuffer::Create(vertices, sizeof(vertices));
		m_VertexBuffer->Bind();
		// 创建 & 启用布局layout
		std::shared_ptr<Snail::BufferLayout> layout = Snail::BufferLayout::Create(
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
		m_IndexBuffer = Snail::IndexBuffer::Create(indices, sizeof(indices));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);


		m_Shader = Snail::Shader::Create("shaders/test.shader");

		m_CameraController = std::make_unique<Snail::PerspectiveCameraController>(45.0f, 1920.0f/1080.0f, glm::vec3(0.0f, 0.0f, 3.0f));
		//------------------------------------------------------------------------------
	}
	virtual void OnDetach() override {

	}

	inline void OnUpdate(const Snail::Timestep& ts) override {

		m_CameraController->OnUpdate(ts);
		
		// -------------------临时------------------------------------------
		Snail::Renderer::BeginScene(m_CameraController->GetCamera());

		// 5. 渲染
		// --- 设置 model 矩阵 ---
		// --- 设置 view 矩阵 ---
		// --- 设置 projection 矩阵 --- 都在submit实现
		glm::mat4 model = glm::mat4(1.0f);
		for (int i = 20; i > 0; i--) {
			// 3. 计算变换 (现在有了头文件，这些函数就能用了)
			model = glm::translate(glm::mat4(1.0f), glm::vec3((i-20) * 1.0f)) * 
				glm::rotate(glm::mat4(1.0f), glm::radians(i * 50.0f), glm::vec3(i * 0.5f, i * 1.0f, i * 0.5f)) * 
				glm::scale(glm::mat4(1.0f), glm::vec3(i * 0.05, i * 0.05, i * 0.05));
			Snail::Renderer::Submit(m_Shader, m_VertexArray, model);
		}

		Snail::Renderer::EndScene();
		//----------------------------------------------------------------
	}

	inline void OnEvent(Snail::Event& e) {
		//SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
		m_CameraController->OnEvent(e);
	}

	inline void OnRender() override {
		//SNL_TRACE("ExampleLayer 调用: OnRender()");

	}
};

