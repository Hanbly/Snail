#pragma once

#include "Snail.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// TODO:remove
#include "GLFW/glfw3.h"

class ExampleLayer : public Snail::Layer
{
private:
	// -------------------临时------------------------------------------
	Snail::Refptr<Snail::VertexArray> m_VertexArray;
	Snail::Refptr<Snail::VertexBuffer> m_VertexBuffer;
	Snail::Refptr<Snail::IndexBuffer> m_IndexBuffer;
	//Snail::Refptr<Snail::Shader> m_Shader;
	Snail::ShaderLibrary m_ShaderLibrary;
	Snail::Refptr<Snail::Texture> m_Texture1; // 纹理 1
	Snail::Refptr<Snail::Texture> m_Texture2; // 纹理 2
	Snail::Uniptr<Snail::PerspectiveCameraController> m_CameraController;
	glm::vec3 u_LightPosition = glm::vec3(0.0f, 0.0f, -5.0f);
	glm::vec4 u_LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
	float u_MixValue = 0.0f;
	//------------------------------------------------------------------
public:
	ExampleLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	virtual void OnAttach() override {
		SNL_PROFILE_FUNCTION();
		// -------------------临时------------------------------------------
		m_VertexArray = Snail::VertexArray::Create();
		m_VertexArray->Bind();

		float vertices[] = {
			// 格式: Position (x, y, z), TexCoords (u, v)

			// 1. 前面 (Front Face) - Z = 0.5f
			// 位置				  // 纹理坐标	   // 法向量
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 0 左下
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // 1 右下
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // 2 右上
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f, // 3 左上

			// 2. 右面 (Right Face) - X = 0.5f
			// 位置				  // 纹理坐标	   // 法向量
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 4 左下 (对应前面的右下空间位置)
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 5 右下
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f, // 6 右上
			 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f, // 7 左上

			 // 3. 后面 (Back Face) - Z = -0.5f (注意：为了看纹理是正的，UV顺序做了调整)
			  // 位置			   // 纹理坐标	// 法向量
			  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 8
			 -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 9
			 -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 10
			  0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 11

			  // 4. 左面 (Left Face) - X = -0.5f
			   // 位置				// 纹理坐标	 // 法向量
			  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 12
			  -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 13
			  -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 14
			  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 15

			  // 5. 上面 (Top Face) - Y = 0.5f
			  // 位置				// 纹理坐标	 // 法向量
			  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 16
			   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 17
			   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 18
			  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f, // 19

			  // 6. 下面 (Bottom Face) - Y = -0.5f
			  // 位置				// 纹理坐标	 // 法向量
			  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 20
			   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 21
			   0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 22
			  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 23
		};
		m_VertexBuffer = Snail::VertexBuffer::Create(vertices, sizeof(vertices));
		m_VertexBuffer->Bind();
		// 创建 & 启用布局layout
		Snail::Refptr<Snail::BufferLayout> layout = Snail::BufferLayout::Create(
			{
				{ "positions", Snail::VertexDataType::Float3 },
				{ "texture_coords", Snail::VertexDataType::Float2 },
				{ "normal", Snail::VertexDataType::Float3 }
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

		//m_Shader = Snail::Shader::Create("assets/shaders/cube.shader");
		m_ShaderLibrary.Load("cube", "assets/shaders/cube.glsl");
		m_ShaderLibrary.Load("light_box", "assets/shaders/light_box.glsl");

		m_Texture1 = Snail::Texture2D::Create("assets/images/kulisu.png");
		m_Texture2 = Snail::Texture2D::Create("assets/images/mayoli.png");

		m_CameraController = std::make_unique<Snail::PerspectiveCameraController>(45.0f, 1920.0f/1080.0f, glm::vec3(0.0f, 0.0f, 3.0f));
		//------------------------------------------------------------------------------
	}
	virtual void OnDetach() override {

	}

	inline virtual void OnUpdate(const Snail::Timestep& ts) override {

		SNL_PROFILE_FUNCTION();

		m_CameraController->OnUpdate(ts);
	}

	inline virtual void OnEvent(Snail::Event& e) {
		//SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
		m_CameraController->OnEvent(e);
	}

	inline virtual void OnRender() override {
		//SNL_TRACE("ExampleLayer 调用: OnRender()");
		// -------------------临时------------------------------------------
		Snail::Renderer::BeginScene(m_CameraController->GetCamera());

		// 5. 渲染
		// 设置uniform
		auto testShader = m_ShaderLibrary.Get("cube");
		testShader->Bind();
		testShader->SetFloat4("u_LightColor", u_LightColor);
		testShader->SetInt("u_Texture1", 0);
		testShader->SetInt("u_Texture2", 1);
		testShader->SetFloat("u_MixValue", u_MixValue);
		testShader->SetFloat3("u_LightPosition", u_LightPosition);
		testShader->SetFloat3("u_ViewPosition", m_CameraController->GetCamera()->GetCameraPos());

		auto lightShader = m_ShaderLibrary.Get("light_box");
		lightShader->Bind();
		lightShader->SetFloat4("u_LightColor", u_LightColor); // 白色光照

		// ============================================================
		// 1. 更新光源位置 (自动绕 Y 轴旋转)
		// ============================================================
		// 半径
		float radius = 4.0f;
		// 速度 (如果你想用 accumulated time，可以用 m_Time += ts; 替代 glfwGetTime)
		float timeValue = (float)glfwGetTime();

		// 让光源在 XZ 平面上做圆周运动，高度 Y 保持不变(或者微调)
		u_LightPosition.x = sin(timeValue) * radius;
		u_LightPosition.z = cos(timeValue) * radius;
		// u_LightPosition.y = 2.0f; // 如果你想固定高度，可以取消注释这行

		// ============================================================
		// 2. 渲染中间的 "主角" 立方体 (Hero Cube)
		// ============================================================
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 放在正中心，稍微旋转一点展示立体感
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
			// 稍微大一点
			model = glm::scale(model, glm::vec3(1.5f));

			// 绑定纹理
			m_Texture1->Bind(0);
			m_Texture2->Bind(1);

			// 提交给 Test Shader (带光照计算的)
			testShader->Bind();
			testShader->SetFloat3("u_LightPosition", u_LightPosition); // !!! 关键：更新Shader里的光源位置
			Snail::Renderer::Submit(testShader, m_VertexArray, model);
		}

		// ============================================================
		// 3. 渲染四周的 4 个参照立方体 (Satellites)
		// ============================================================
		// 定义4个位置：左、右、上、下 (或者前后左右)
		glm::vec3 cubePositions[] = {
			glm::vec3(3.0f,  0.0f,  0.0f),
			glm::vec3(-3.0f,  0.0f,  0.0f),
			glm::vec3(0.0f,  3.0f,  0.0f),
			glm::vec3(0.0f, -3.0f,  0.0f)
		};

		for (int i = 0; i < 4; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			// 让它们以不同的角度旋转，这样能看到不同面的反光
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(0.8f)); // 稍微小一点

			Snail::Renderer::Submit(testShader, m_VertexArray, model);
		}

		// ============================================================
		// 4. 渲染光源本身 (Light Cube)
		// ============================================================
		{
			auto lightShader = m_ShaderLibrary.Get("light_box");
			lightShader->Bind();
			lightShader->SetFloat4("u_LightColor", u_LightColor);

			glm::mat4 model = glm::mat4(1.0f);
			// 直接使用计算好的动态位置
			model = glm::translate(model, glm::vec3(u_LightPosition.x, u_LightPosition.y, u_LightPosition.z));
			model = glm::scale(model, glm::vec3(0.2f)); // 光源做小一点，看起来像灯泡

			Snail::Renderer::Submit(lightShader, m_VertexArray, model);
		}
		//----------------------------------------------------------------
	}

	inline virtual void OnImGuiRender() override {
		ImGui::Begin("Settings");

		ImGui::SliderFloat3("Light Position", glm::value_ptr(u_LightPosition), -10.0f, 10.0f);
		ImGui::ColorEdit4("Light Color", glm::value_ptr(u_LightColor));
		ImGui::SliderFloat("Alpha Slider", &u_MixValue, 0.0f, 1.0f, "%.1f");

		for (auto& result : Snail::s_ProfilingResults)
		{
			// 计算持续时间 (毫秒)
			float duration = (result.end - result.start) * 0.001f;

			// 显示
			ImGui::Text("%.3fms,  %s", duration, result.name.c_str());
		}

		ImGui::End();
	}
};

