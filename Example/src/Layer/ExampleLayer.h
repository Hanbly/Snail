#pragma once

#include "Snail.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ExampleLayer : public Snail::Layer
{
private:
	// -------------------临时------------------------------------------
	//Snail::Refptr<Snail::VertexArray> m_VertexArray;
	//Snail::Refptr<Snail::VertexBuffer> m_VertexBuffer;
	//Snail::Refptr<Snail::IndexBuffer> m_IndexBuffer;
	//Snail::Refptr<Snail::Shader> m_Shader;
	Snail::ShaderLibrary m_ShaderLibrary;
	//Snail::Refptr<Snail::Texture> m_Texture1; // 纹理 1
	//Snail::Refptr<Snail::Texture> m_Texture2; // 纹理 2
	//Snail::Refptr<Snail::Material> m_CubeMaterial;
	//Snail::Refptr<Snail::Material> m_LightMaterial;
	Snail::Refptr<Snail::Mesh> m_CubeMesh;
	Snail::Refptr<Snail::Mesh> m_LightMesh;
	Snail::Refptr<Snail::Model> m_Model;
	
	Snail::Uniptr<Snail::PerspectiveCameraController> m_CameraController;
	glm::vec3 u_LightPosition = glm::vec3(0.0f, 100.0f, 0.0f);
	glm::vec4 u_LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
	float u_MixValue = 0.0f;

	float u_AmbientStrength = 0.1f;     // 环境光照系数
	float u_DiffuseStrength = 0.8f;     // 漫反射系数
	float u_SpecularStrength = 0.5f;    // 镜面反射系数
	float u_Shininess = 32.0f;           // 反光度
	//------------------------------------------------------------------
public:
	ExampleLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	virtual void OnAttach() override {
		SNL_PROFILE_FUNCTION();
		// -------------------临时------------------------------------------
		std::vector<Snail::Vertex> vertices = {
			// 格式需对应 Vertex 结构体定义: { Position, Normal, TexCoord }
			// 1. 前面 (Front Face) - Z = 0.5f
			// Pos                          // Normal           // UV
			{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f} },
			{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f} },
			{ { 0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f} },
			{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f} },

			// 2. 右面 (Right Face) - X = 0.5f
			{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 0.0f} },
			{ { 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f} },
			{ { 0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f},  {1.0f, 1.0f} },
			{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f} },

			// 3. 后面 (Back Face) - Z = -0.5f
			{ { 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
			{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
			{ {-0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f} },
			{ { 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f} },

			// 4. 左面 (Left Face) - X = -0.5f
			{ {-0.5f, -0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
			{ {-0.5f, -0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} },
			{ {-0.5f,  0.5f,  0.5f},  {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
			{ {-0.5f,  0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },

			// 5. 上面 (Top Face) - Y = 0.5f
			{ {-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f} },
			{ { 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f} },
			{ { 0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f} },
			{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f} },

			// 6. 下面 (Bottom Face) - Y = -0.5f
			{ {-0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
			{ { 0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
			{ { 0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
			{ {-0.5f, -0.5f,  0.5f},  {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f} }
		};
		std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0,       // 前面
			4, 5, 6, 6, 7, 4,       // 右面
			8, 9, 10, 10, 11, 8,    // 后面
			12, 13, 14, 14, 15, 12, // 左面
			16, 17, 18, 18, 19, 16, // 上面
			20, 21, 22, 22, 23, 20  // 下面
		};

		m_ShaderLibrary.Load("cube", "assets/shaders/cube.glsl");
		m_ShaderLibrary.Load("light_box", "assets/shaders/light_box.glsl");
		m_ShaderLibrary.Load("model", "assets/shaders/Model_Shader.glsl");
		

		std::vector<Snail::TextureData> td;
		td.push_back(Snail::TextureData(Snail::Texture2D::Create("assets/images/kulisu.png"), "texture_diffuse"));
		td.push_back(Snail::TextureData(Snail::Texture2D::Create("assets/images/mayoli.png"), "texture_diffuse"));
		m_CubeMesh = std::make_shared<Snail::Mesh>(vertices, indices, m_ShaderLibrary.Get("cube"), td);
		m_LightMesh = std::make_shared<Snail::Mesh>(vertices, indices, m_ShaderLibrary.Get("light_box"));

		m_CubeMesh->GetMaterial()->SetFloat("u_MixValue", u_MixValue);
		m_CubeMesh->GetMaterial()->SetFloat("u_AmbientStrength", u_AmbientStrength);
		m_CubeMesh->GetMaterial()->SetFloat("u_DiffuseStrength", u_DiffuseStrength);
		m_CubeMesh->GetMaterial()->SetFloat("u_SpecularStrength", u_SpecularStrength);
		m_CubeMesh->GetMaterial()->SetFloat("u_Shininess", u_Shininess);

		//m_Model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/bugatti/bugatti.obj");
		//m_Model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/dragon/dragon.obj");
		//m_Model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/sportsCar/sportsCar.obj");
		m_Model = std::make_shared<Snail::Model>(m_ShaderLibrary.Get("model"), "assets/models/sponza/sponza.obj");


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
		// 
		{
			for (const auto& mesh : m_Model->GetMeshs()) {
				mesh->GetMaterial()->SetFloat("u_AmbientStrength", u_AmbientStrength);
				mesh->GetMaterial()->SetFloat("u_DiffuseStrength", u_DiffuseStrength);
				mesh->GetMaterial()->SetFloat("u_SpecularStrength", u_SpecularStrength);
			}

			glm::mat4 transform = glm::mat4(1.0f);
			// 放在正中心
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
			transform = glm::scale(transform, glm::vec3(1.0f));
			m_Model->Draw(transform);
		}
		// 画方块，只需传位置
		{
			m_CubeMesh->GetMaterial()->SetFloat("u_MixValue", u_MixValue);
			m_CubeMesh->GetMaterial()->SetFloat("u_AmbientStrength", u_AmbientStrength);
			m_CubeMesh->GetMaterial()->SetFloat("u_DiffuseStrength", u_DiffuseStrength);
			m_CubeMesh->GetMaterial()->SetFloat("u_SpecularStrength", u_SpecularStrength);
			m_CubeMesh->GetMaterial()->SetFloat("u_Shininess", u_Shininess);

			glm::mat4 transform = glm::mat4(1.0f);
			// 放在正中心，稍微旋转一点展示立体感
			transform = glm::translate(transform, glm::vec3(0.0f, 20.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
			transform = glm::scale(transform, glm::vec3(25.0f));
			m_CubeMesh->Draw(transform);
		}

		// 画光源 (用另一个简单的材质)
		{
			glm::mat4 lightTransform = glm::mat4(1.0f);
			lightTransform = glm::translate(lightTransform, glm::vec3(u_LightPosition.x, u_LightPosition.y, u_LightPosition.z));
			lightTransform = glm::scale(lightTransform, glm::vec3(10.0f));
			m_LightMesh->Draw(lightTransform);
		}

		//----------------------------------------------------------------
	}

	inline virtual void OnImGuiRender() override {
		ImGui::Begin(u8"设置");

		// 关键：检测的变量必须在循环中维持更新
		ImGui::SliderFloat3(u8"光源位置坐标", glm::value_ptr(u_LightPosition), -200.0f, 500.0f);
		ImGui::ColorEdit4(u8"光线颜色", glm::value_ptr(u_LightColor));
		ImGui::SliderFloat(u8"纹理变换", &u_MixValue, 0.0f, 1.0f, "%.1f");
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

