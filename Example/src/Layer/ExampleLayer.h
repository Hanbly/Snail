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
	std::unique_ptr<Snail::Camera> m_Camera;

	float m_LastMouseX;
	float m_LastMouseY;
	bool  m_FirstMouse = true; // 第一帧标志位
	float m_MouseSensitivity = 0.1f; // 灵敏度
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

	inline void OnUpdate(const Snail::Timestep& ts) override {
		const float& time = ts.GetSeconds();
		if (Snail::Input::IsKeyPressed(SNL_KEY_W)) {
			m_Camera->MoveCamera(Snail::Camera::TranslationDirection::FRONT, m_Camera->GetMoveSpeed() * time);
		}
		else if (Snail::Input::IsKeyPressed(SNL_KEY_S)) {
			m_Camera->MoveCamera(Snail::Camera::TranslationDirection::BACK, m_Camera->GetMoveSpeed() * time);
		}
		if (Snail::Input::IsKeyPressed(SNL_KEY_E)) {
			m_Camera->MoveCamera(Snail::Camera::TranslationDirection::UP, m_Camera->GetMoveSpeed() * time);
		}
		else if (Snail::Input::IsKeyPressed(SNL_KEY_Q)) {
			m_Camera->MoveCamera(Snail::Camera::TranslationDirection::DOWN, m_Camera->GetMoveSpeed() * time);
		}
		if (Snail::Input::IsKeyPressed(SNL_KEY_A)) {
			m_Camera->MoveCamera(Snail::Camera::TranslationDirection::LEFT, m_Camera->GetMoveSpeed() * time);
		}
		else if (Snail::Input::IsKeyPressed(SNL_KEY_D)) {
			m_Camera->MoveCamera(Snail::Camera::TranslationDirection::RIGHT, m_Camera->GetMoveSpeed() * time);
		}

		// -------------------临时------------------------------------------
		Snail::Renderer::BeginScene(m_Camera);

		// 2. 创建变换矩阵
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// 3. 计算变换 (现在有了头文件，这些函数就能用了)
		model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.5f));
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
		Snail::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Snail::MouseMoveEvent>(BIND_NSTATIC_MEMBER_Fn(ExampleLayer::OnMouseMoveEvent));
	}

	inline bool OnMouseMoveEvent(Snail::MouseMoveEvent& e) {
		if (m_FirstMouse) {
			m_LastMouseX = e.GetMouseX();
			m_LastMouseY = e.GetMouseY();
			m_FirstMouse = false;
			return false;
		}
		float xoffset = e.GetMouseX() - m_LastMouseX;
		float yoffset = m_LastMouseY - e.GetMouseY();
		m_LastMouseX = e.GetMouseX();
		m_LastMouseY = e.GetMouseY();

		xoffset *= m_MouseSensitivity;
		yoffset *= m_MouseSensitivity;

		if (Snail::Input::IsMouseButton(SNL_MOUSE_BUTTON_MIDDLE)) {
			m_Camera->RotateCamera(xoffset, yoffset);
		}
		return false;
	}

	inline void OnRender() override {
		//SNL_TRACE("ExampleLayer 调用: OnRender()");
	}
};

