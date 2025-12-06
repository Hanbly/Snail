#include "SNLpch.h"

#include "Renderer3D.h"

namespace Snail {

	Renderer3D::Renderer3DSceneData Renderer3D::s_3DSceneData;

	void Renderer3D::Init()
	{
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::BeginScene(const Uniptr<Camera>& camera, const glm::vec3& lightPos, const glm::vec4& lightColor)
	{
		SNL_PROFILE_FUNCTION();


		// 计算 View * Projection
		s_3DSceneData.ViewProjectionMatrix = camera->GetProjectionMatrix() * camera->GetViewMatrix();

		// 获取相机位置
		s_3DSceneData.CameraPosition = camera->GetCameraPos();

		s_3DSceneData.LightPosition = lightPos;
		s_3DSceneData.LightColor = lightColor;
	}

	void Renderer3D::EndScene()
	{
		SNL_PROFILE_FUNCTION();
	}

	void Renderer3D::DrawMesh(const Refptr<VertexArray>& vertexArray, const Refptr<Material>& material, const glm::mat4& transform)
	{
		SNL_PROFILE_FUNCTION();


		// 绑定 Shader，绑定纹理，上传材质特有的 Uniform
		material->Bind();

		// 这些是所有物体共用的，但在 Draw 时需要设置给当前绑定的 Shader
		auto shader = material->GetShader();

		shader->SetMat4("u_ViewProjection", s_3DSceneData.ViewProjectionMatrix);
		shader->SetMat4("u_Model", transform);
		shader->SetMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(transform))));

		shader->SetFloat3("u_ViewPosition", s_3DSceneData.CameraPosition);
		shader->SetFloat3("u_LightPosition", s_3DSceneData.LightPosition);
		shader->SetFloat4("u_LightColor", s_3DSceneData.LightColor);

		// 绘制几何体
		vertexArray->Bind();
		RendererCommand::DrawIndexed(vertexArray);
	}

}