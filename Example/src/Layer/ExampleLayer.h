#pragma once

#include "Snail.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ExampleLayer : public Snail::Layer
{
private:
	// -------------------临时------------------------------------------
	Snail::Refptr<Snail::VertexArray> m_VertexArray;
	Snail::Refptr<Snail::VertexBuffer> m_VertexBuffer;
	Snail::Refptr<Snail::IndexBuffer> m_IndexBuffer;
	Snail::Refptr<Snail::Shader> m_Shader;
	Snail::Refptr<Snail::Texture> m_Texture1; // 纹理 1
	Snail::Refptr<Snail::Texture> m_Texture2; // 纹理 2
	Snail::Uniptr<Snail::PerspectiveCameraController> m_CameraController;
	glm::vec4 u_DeltaColor = {0.8f, 0.8f, 0.8f, 1.0f};
	float u_MixValue = 0.0f;
	//------------------------------------------------------------------
public:
	ExampleLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	virtual void OnAttach() override {
		// -------------------临时------------------------------------------
		m_VertexArray = Snail::VertexArray::Create();
		m_VertexArray->Bind();

		float vertices[] = {
			// 格式: Position (x, y, z), TexCoords (u, v)

			// 1. 前面 (Front Face) - Z = 0.5f
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 0 左下
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 1 右下
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 2 右上
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 3 左上

			// 2. 右面 (Right Face) - X = 0.5f
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 4 左下 (对应前面的右下空间位置)
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 5 右下
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 6 右上
			 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 7 左上

			 // 3. 后面 (Back Face) - Z = -0.5f (注意：为了看纹理是正的，UV顺序做了调整)
			  0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 8
			 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 9
			 -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 10
			  0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 11

			  // 4. 左面 (Left Face) - X = -0.5f
			  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 12
			  -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 13
			  -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 14
			  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 15

			  // 5. 上面 (Top Face) - Y = 0.5f
			  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // 16
			   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 17
			   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 18
			  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 19

			  // 6. 下面 (Bottom Face) - Y = -0.5f
			  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 20
			   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 21
			   0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // 22
			  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f  // 23
		};
		m_VertexBuffer = Snail::VertexBuffer::Create(vertices, sizeof(vertices));
		m_VertexBuffer->Bind();
		// 创建 & 启用布局layout
		Snail::Refptr<Snail::BufferLayout> layout = Snail::BufferLayout::Create(
			{
				{ "positions", Snail::VertexDataType::Float3 },
				{ "texture_coords", Snail::VertexDataType::Float2 }
			}
		);
		m_VertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		uint32_t indices[] = {
			0, 1, 2, 2, 3, 0,       // 前面
			4, 5, 6, 6, 7, 4,       // 右面
			8, 9, 10, 10, 11, 8,    // 后面
			12, 13, 14, 14, 15, 12, // 左面
			16, 17, 18, 18, 19, 16, // 上面
			20, 21, 22, 22, 23, 20  // 下面
		};
		m_IndexBuffer = Snail::IndexBuffer::Create(indices, sizeof(indices));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);


		m_Shader = Snail::Shader::Create("assets/shaders/test.shader");

		m_Texture1 = Snail::Texture2D::Create("assets/images/kulisu.png");
		m_Texture2 = Snail::Texture2D::Create("assets/images/mayoli.png");

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
		// 设置uniform
		m_Shader->SetUniform4f("u_Color", u_DeltaColor);
		m_Shader->SetUniform1i("u_Texture1", 0);
		m_Shader->SetUniform1i("u_Texture2", 1);
		m_Shader->SetUniform1f("u_MixValue", u_MixValue);
		
		// --- 设置 model 矩阵 ---
		// --- 设置 view 矩阵 ---
		// --- 设置 projection 矩阵 --- 都在submit实现
		glm::mat4 model = glm::mat4(1.0f);
		for (int i = 20; i > 0; i--) {
			// 3. 计算变换 (现在有了头文件，这些函数就能用了)
			glm::mat4 translate = glm::mat4(1.0f), rotate = glm::mat4(1.0f), scale = glm::mat4(1.0f);
			translate = glm::translate(glm::mat4(1.0f), glm::vec3((i - 20) * 1.0f));
			//rotate = glm::rotate(glm::mat4(1.0f), glm::radians(i * 50.0f), glm::vec3(i * 0.5f, i * 1.0f, i * 0.5f));
			scale = glm::scale(glm::mat4(1.0f), glm::vec3(i * 0.05, i * 0.05, i * 0.05));
			model = translate * rotate * scale;

			m_Texture1->Bind(0); // 绑定 kulisu 到槽位 0
			m_Texture2->Bind(1); // 绑定 mayoli 到槽位 1

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
		OnImGuiRender();
	}

	inline void OnImGuiRender() {
		ImGui::Begin("Settings");

		ImGui::ColorEdit4("cubes_color", glm::value_ptr(u_DeltaColor));
		ImGui::SliderFloat("alphaSlider", &u_MixValue, 0.0f, 1.0f, "%.1f");

		ImGui::End();
	}
};

