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
	//Snail::Refptr<Snail::Shader> m_Shader;
	Snail::ShaderLibrary m_ShaderLibrary;
	Snail::Refptr<Snail::Texture> m_Texture1; // 纹理 1
	Snail::Refptr<Snail::Texture> m_Texture2; // 纹理 2
	Snail::Refptr<Snail::Material> m_CubeMaterial;
	Snail::Refptr<Snail::Material> m_LightMaterial;
	Snail::Uniptr<Snail::PerspectiveCameraController> m_CameraController;
	glm::vec3 u_LightPosition = glm::vec3(0.0f, 0.0f, -5.0f);
	glm::vec4 u_LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
	float u_MixValue = 0.0f;

	float u_AmbientStrength = 0.1f;     // 环境光照系数
	float u_DiffuseStrength = 0.5f;     // 漫反射系数
	float u_SpecularStrength = 0.5f;    // 镜面反射系数
	float u_Shininess = 64.0f;           // 反光度
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




		m_ShaderLibrary.Load("cube", "assets/shaders/cube.glsl");
		m_ShaderLibrary.Load("light_box", "assets/shaders/light_box.glsl");

		m_Texture1 = Snail::Texture2D::Create("assets/images/kulisu.png");
		m_Texture2 = Snail::Texture2D::Create("assets/images/mayoli.png");

		// 材质
		m_CubeMaterial = Snail::Material::Create(m_ShaderLibrary.Get("cube"));
		m_CubeMaterial->SetTexture("u_Texture1", m_Texture1);
		m_CubeMaterial->SetTexture("u_Texture2", m_Texture2);
		m_CubeMaterial->SetFloat("u_MixValue", u_MixValue);
		m_CubeMaterial->SetFloat("u_AmbientStrength", u_AmbientStrength);
		m_CubeMaterial->SetFloat("u_DiffuseStrength", u_DiffuseStrength);
		m_CubeMaterial->SetFloat("u_SpecularStrength", u_SpecularStrength);
		m_CubeMaterial->SetFloat("u_Shininess", u_Shininess);

		m_LightMaterial = Snail::Material::Create(m_ShaderLibrary.Get("light_box"));

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
		Snail::Renderer3D::BeginScene(m_CameraController->GetCamera(), u_LightPosition, u_LightColor);

		// 5. 渲染
		// 画方块，只需传位置
		{
			m_CubeMaterial->SetFloat("u_MixValue", u_MixValue);
			m_CubeMaterial->SetFloat("u_AmbientStrength", u_AmbientStrength);
			m_CubeMaterial->SetFloat("u_DiffuseStrength", u_DiffuseStrength);
			m_CubeMaterial->SetFloat("u_SpecularStrength", u_SpecularStrength);
			m_CubeMaterial->SetFloat("u_Shininess", u_Shininess);

			glm::mat4 transform = glm::mat4(1.0f);
			// 放在正中心，稍微旋转一点展示立体感
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
			transform = glm::scale(transform, glm::vec3(1.5f));
			Snail::Renderer3D::DrawMesh(m_VertexArray, m_CubeMaterial, transform);
		}

		// 画光源 (用另一个简单的材质)
		{
			glm::mat4 lightTransform = glm::mat4(1.0f);
			lightTransform = glm::translate(lightTransform, glm::vec3(u_LightPosition.x, u_LightPosition.y, u_LightPosition.z));
			lightTransform = glm::scale(lightTransform, glm::vec3(0.2f));
			Snail::Renderer3D::DrawMesh(m_VertexArray, m_LightMaterial, lightTransform);
		}

		//----------------------------------------------------------------
	}

	inline virtual void OnImGuiRender() override {
		ImGui::Begin(u8"设置");

		// 关键：检测的变量必须在循环中维持更新
		ImGui::SliderFloat3(u8"光源位置坐标", glm::value_ptr(u_LightPosition), -10.0f, 10.0f);
		ImGui::ColorEdit4(u8"光线颜色", glm::value_ptr(u_LightColor));
		ImGui::SliderFloat(u8"物体变换", &u_MixValue, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat(u8"环境光照系数", &u_AmbientStrength, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat(u8"漫反射系数", &u_DiffuseStrength, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat(u8"镜面反射系数", &u_SpecularStrength, 0.0f, 1.0f, "%.1f");
		ImGui::SliderFloat(u8"反光度", &u_Shininess, 0.0f, 256.0f, "%.1f");

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

