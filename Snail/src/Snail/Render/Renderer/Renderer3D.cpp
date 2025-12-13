#include "SNLpch.h"

#include "Renderer3D.h"

namespace Snail {

	Renderer3D::Renderer3DSceneData Renderer3D::s_3DSceneData;

	void Renderer3D::Init()
	{
		s_3DSceneData.ShaderLibrary.Load("single_color", "assets/shaders/single_color.glsl");
		s_3DSceneData.ShaderLibrary.Load("edge_shader", "assets/shaders/edge_shader.glsl");
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform, const glm::vec3& lightPos, const glm::vec4& lightColor)
	{
		SNL_PROFILE_FUNCTION();


		// 计算 View * Projection
		s_3DSceneData.ViewProjectionMatrix = camera.GetProjectionMatrix() * glm::inverse(transform);

		// 获取相机位置
		s_3DSceneData.CameraPosition = glm::vec3(transform[3]);

		s_3DSceneData.LightPosition = lightPos;
		s_3DSceneData.LightColor = lightColor;
	}

	void Renderer3D::EndScene()
	{
		SNL_PROFILE_FUNCTION();
	}

	void Renderer3D::DrawMesh(const Mesh& mesh, const bool& edgeEnable, const glm::mat4& transform)
	{
		SNL_PROFILE_FUNCTION();


		// 绑定 Shader，绑定纹理，上传材质特有的 Uniform
		mesh.GetMaterial()->Bind();

		// 这些是所有物体共用的，但在 Draw 时需要设置给当前绑定的 Shader
		auto shader = mesh.GetMaterial()->GetShader();

		shader->SetMat4("u_ViewProjection", s_3DSceneData.ViewProjectionMatrix);
		shader->SetMat4("u_Model", transform);
		shader->SetMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(transform))));

		shader->SetFloat3("u_ViewPosition", s_3DSceneData.CameraPosition);
		shader->SetFloat3("u_LightPosition", s_3DSceneData.LightPosition);
		shader->SetFloat4("u_LightColor", s_3DSceneData.LightColor);

		// 绘制几何体
		mesh.GetVAO()->Bind();

		RendererCommand::StencilMask(edgeEnable);
		RendererCommand::StencilFunc(RendererCommand::StencilFuncType::ALWAYS, 1, 0xFF);

		RendererCommand::DrawIndexed(mesh.GetVAO());

		if (edgeEnable) {
			//RendererCommand::DepthTest(false);
			RendererCommand::StencilMask(false); // 不再写入模板，只读取模板信息
			RendererCommand::StencilFunc(RendererCommand::StencilFuncType::NOTEQUAL, 1, 0xFF);

			// 临时边框shader
			auto edgeshader = s_3DSceneData.ShaderLibrary.Get("single_color");
			edgeshader->Bind();
			edgeshader->SetMat4("u_Model", glm::scale(transform, glm::vec3(1.03f, 1.03f, 1.03f)));
			edgeshader->SetMat4("u_ViewProjection", s_3DSceneData.ViewProjectionMatrix);
			//edgeshader->SetFloat("u_Edge", 0.05f); // 边框厚度 (法线外扩方法再使用)
			edgeshader->SetFloat4("u_Color", { 1.0f, 0.5f, 0.0f, 1.0f }); // 橙色边框

			// 绘制放大后的物体
			// 注意：这里需要再次调用 Draw，但是用新的 Shader 和 放大后的 Matrix
			mesh.GetVAO()->Bind();
			RendererCommand::DrawIndexed(mesh.GetVAO());

			// 恢复状态 (非常重要，否则后续渲染会乱)
			RendererCommand::DepthTest(true);
			RendererCommand::StencilMask(true);
			RendererCommand::StencilFunc(RendererCommand::StencilFuncType::ALWAYS, 1, 0xFF);
		}
	}

	void Renderer3D::DrawModel(const Model& model, const bool& edgeEnable, const glm::mat4& transform)
	{
		SNL_PROFILE_FUNCTION();


		for (const Refptr<Mesh> mesh : model.GetMeshs()) {
			DrawMesh(*mesh, edgeEnable, transform);
		}
	}

}