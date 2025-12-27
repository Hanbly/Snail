#include "SNLpch.h"

#include "Material/ShaderLibrary.h"

#include "Renderer3D.h"

#include <glm/gtx/norm.hpp>

namespace Snail {

	Renderer3D::Renderer3DSceneData Renderer3D::s_3DSceneData;

	void Renderer3D::Init()
	{
		// 创建实例 VBO，大小为 MaxInstances * (sizeof(glm::mat4) + sizeof(glm::mat3))
		s_3DSceneData.InstanceVBO = VertexBuffer::Create(MAX_INSTANCES_PER_BATCH * sizeof(InstanceData));
		// 设置布局
		s_3DSceneData.InstanceVBO->SetLayout(BufferLayout::Create({ 
			{ "a_Model", VertexDataType::Mat4 },
			{ "a_NormalMatrix", VertexDataType::Mat3 }, // 法线矩阵
			{ "a_EntityID", VertexDataType::Int }, // location 10

			{ "a_Padding", VertexDataType::Float2 }
			}));

		ShaderLibrary::Load("PostProcessOutline", "assets/shaders/process_outline.glsl", {});
	}

	void Renderer3D::Shutdown()
	{
	}

	void Renderer3D::BeginScene(const Camera* camera, const glm::mat4& transform, std::vector<DirectionLight>& dirLights, std::vector<PointLight>& poiLights)
	{
		SNL_PROFILE_FUNCTION();


		// 计算 View * Projection
		s_3DSceneData.ViewProjectionMatrix = camera->GetProjection() * glm::inverse(transform);

		// 获取相机位置
		s_3DSceneData.CameraPosition = glm::vec3(transform[3]);

		// 复制光源列表
		s_3DSceneData.DirLights.assign(dirLights.begin(), dirLights.end());
		s_3DSceneData.PoiLights.assign(poiLights.begin(), poiLights.end());
	}

	void Renderer3D::EndScene()
	{
		SNL_PROFILE_FUNCTION();

		if(s_3DSceneData.EnableInstancing)
			FlushMeshes();
	}

	void Renderer3D::DrawSkybox(const Model& model, const EditorCamera& camera)
	{
		SNL_PROFILE_FUNCTION();


		// Skybox 的深度值通常是 1.0 (最远)，所以需要 GL_LEQUAL (小于等于) 让它通过深度测试
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::LEQUAL);
		// 可选，取决于是否还要在其后渲染透明物体
		// RenderCommand::DepthMask(false); 

		auto& mesh = model.GetMeshes()[0];
		auto& material = mesh->GetMaterial();
		auto& shader = material->GetShader();

		material->Bind(); // 绑定纹理

		// 计算特殊的 ViewProjection 矩阵
		// 移除 View 矩阵的平移分量，只保留旋转
		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 projection = camera.GetProjection();
		glm::mat4 viewProj = projection * view;

		shader->SetMat4("u_ViewProjection", viewProj);

		// 绘制
		mesh->GetVAO()->Bind();
		RendererCommand::DrawIndexed(mesh->GetVAO());

		// 恢复渲染状态 (非常重要！)
		RendererCommand::SetDepthFunc(RendererCommand::DepthFuncType::LESS); // 恢复默认 LESS
		// RenderCommand::DepthMask(true);
	}

	void Renderer3D::DrawMesh(const Mesh& mesh, const bool& edgeEnable, const glm::mat4& transform)
	{
		SNL_PROFILE_FUNCTION();


		// 绑定 Shader，绑定纹理，上传材质特有的 Uniform
		auto& material = mesh.GetMaterial();
		material->GetShader()->Bind();

		material->SetMat4("u_ViewProjection", s_3DSceneData.ViewProjectionMatrix);
		material->SetMat4("u_Model", transform);
		material->SetMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(transform))));

		material->SetFloat3("u_ViewPosition", s_3DSceneData.CameraPosition);

		// 上传光源列表的 uniforms 
		UploadLightsUniforms(material->GetShader());
		//material->SetFloat3("u_LightPosition", s_3DSceneData.LightPosition);
		//material->SetFloat4("u_LightColor", s_3DSceneData.LightColor);
		//material->SetFloat4("u_LightColor", s_3DSceneData.LightColor);
		//material->SetFloat("u_AmbientStrength", s_3DSceneData.AmbientStrength);

		material->SetInt("u_EntityID", (int)edgeEnable); // 后处理边缘的 uniform

		material->Bind();

		// 绘制几何体
		mesh.GetVAO()->Bind();

		//RendererCommand::StencilMask(edgeEnable);
		//RendererCommand::StencilFunc(RendererCommand::StencilFuncType::ALWAYS, 1, 0xFF);

		RendererCommand::DrawIndexed(mesh.GetVAO());

		//if (edgeEnable) {
		//	//RendererCommand::DepthTest(false);
		//	RendererCommand::StencilMask(false); // 不再写入模板，只读取模板信息
		//	RendererCommand::StencilFunc(RendererCommand::StencilFuncType::NOTEQUAL, 1, 0xFF);

		//	// 临时边框shader
		//	auto edgeshader = ShaderLibrary::Get("single_color");
		//	edgeshader->Bind();
		//	edgeshader->SetMat4("u_Model", glm::scale(transform, glm::vec3(1.03f, 1.03f, 1.03f)));
		//	edgeshader->SetMat4("u_ViewProjection", s_3DSceneData.ViewProjectionMatrix);
		//	//edgeshader->SetFloat("u_Edge", 0.05f); // 边框厚度 (法线外扩方法再使用)
		//	edgeshader->SetFloat4("u_Color", { 1.0f, 0.5f, 0.0f, 1.0f }); // 橙色边框

		//	// 绘制放大后的物体
		//	// 注意：这里需要再次调用 Draw，但是用新的 Shader 和 放大后的 Matrix
		//	mesh.GetVAO()->Bind();
		//	RendererCommand::DrawIndexed(mesh.GetVAO());

		//	// 恢复状态 (非常重要，否则后续渲染会乱)
		//	RendererCommand::DepthTest(true);
		//	RendererCommand::StencilMask(true);
		//	RendererCommand::StencilFunc(RendererCommand::StencilFuncType::ALWAYS, 1, 0xFF);
		//}
	}

	void Renderer3D::DrawModel(const Model& model, const bool& edgeEnable, const glm::mat4& transform)
	{
		SNL_PROFILE_FUNCTION();


		for (const Refptr<Mesh> mesh : model.GetMeshes()) {
			if (s_3DSceneData.EnableInstancing) {
				SubmitMesh(mesh, edgeEnable, transform);
			}
			else {
				DrawMesh(*mesh, edgeEnable, transform);
			}
		}
	}

	void Renderer3D::SubmitMesh(const Refptr<Mesh>& mesh, const bool& edgeEnable, const glm::mat4& transform)
	{
		s_3DSceneData.MeshInstanceQueue[mesh].push_back({transform, (int)edgeEnable});
	}

	void Renderer3D::FlushMeshes()
	{
		SNL_PROFILE_FUNCTION();


		static std::vector<InstanceData> s_InstanceBufferData;

		// 遍历所有不同的 Mesh
		for (auto& [mesh, meshData] : s_3DSceneData.MeshInstanceQueue)
		{
			if (meshData.empty()) continue;

			// 绑定材质
			auto& material = mesh->GetMaterial();
			auto oriShader = material->GetShader();
			auto instancingShader = ShaderLibrary::Load(oriShader->GetFilePath(), { "INSTANCING" });
			
			instancingShader->Bind();

			// 设置全局 Uniform (VP矩阵, 光照等)
			// 注意：u_Model 不再通过 Uniform 设置，而是通过 InstanceVBO 传递
			material->SetMat4("u_ViewProjection", s_3DSceneData.ViewProjectionMatrix);

			material->SetFloat3("u_ViewPosition", s_3DSceneData.CameraPosition);

			// 上传光源列表的 uniforms 
			UploadLightsUniforms(instancingShader);

			// 真正上传 uniforms
			material->BindToShader(instancingShader);

			// 绑定 Mesh 的 VAO 并挂载 Instance Buffer
			auto& vao = mesh->GetVAO();
			vao->Bind();

			// 这一步将 InstanceVBO 绑定到该 VAO 的属性位置上 (例如 location 3,4,5,6)
			vao->SetInstanceBuffer(s_3DSceneData.InstanceVBO);

			size_t totalInstances = meshData.size();
			size_t offset = 0;
			while (offset < totalInstances) {

				// 确保不比 MAX_INSTANCES_PER_BATCH 大
				size_t batchCount = std::min((size_t)MAX_INSTANCES_PER_BATCH, totalInstances - offset);

				// -------- 计算实例数据 -------------
				s_InstanceBufferData.clear();
				s_InstanceBufferData.reserve(batchCount);

				for (size_t i = 0; i < batchCount; i++)
				{
					auto& transform = meshData[offset + i].Transform;
					auto& entityId = meshData[offset + i].EntityID;

					InstanceData data;
					data.ModelMatrix = transform;
					// --- 检查矩阵是不是等比缩放 ---
					float sx2 = glm::length2(glm::vec3(transform[0]));
					float sy2 = glm::length2(glm::vec3(transform[1]));
					float sz2 = glm::length2(glm::vec3(transform[2]));
					// 允许一点点误差
					bool isUniform = glm::abs(sx2 - sy2) < 1e-4 && glm::abs(sy2 - sz2) < 1e-4;
					if (isUniform) { // 如果是统一缩放（或者没有缩放），直接取 3x3
						data.NormalMatrix = glm::mat3(transform);
					}
					else { // 需要求逆转置
						data.NormalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
					}

					data.EntityID = entityId;

					s_InstanceBufferData.push_back(data);
				}

				// 上传 ModelMatrix + NormalMatrix + EntityID
				uint32_t dataSize = (uint32_t)s_InstanceBufferData.size() * sizeof(InstanceData);
				s_3DSceneData.InstanceVBO->SetData(s_InstanceBufferData.data(), dataSize);

				// 执行实例化绘制命令
				RendererCommand::DrawIndexedInstanced(vao, (uint32_t)batchCount);

				// 推进偏移量
				offset += batchCount;

			}
		}

		// 清空队列，准备下一帧
		s_3DSceneData.MeshInstanceQueue.clear();
	}

	void Renderer3D::UploadLightsUniforms(const Refptr<Shader>& shader)
	{
		// --- 上传平行光 ---
		int dirLightIndex = 0;
		for (auto& dl : s_3DSceneData.DirLights) {
			// 限制最大光源数量，防止越界（需与 Shader 中的 #define 保持一致）
			if (dirLightIndex >= 4) break;

			std::string baseName = "u_DirLights[" + std::to_string(dirLightIndex) + "]";

			shader->SetFloat3(baseName + ".direction", dl.direction);
			shader->SetFloat3(baseName + ".color", dl.color);
			shader->SetFloat(baseName + ".ambient", dl.ambient);
			shader->SetFloat(baseName + ".diffuse", dl.diffuse);
			shader->SetFloat(baseName + ".specular", dl.specular);

			dirLightIndex++;
		}
		// 告诉 Shader 实际有多少个平行光
		shader->SetInt("u_DirLightCount", dirLightIndex);

		// --- 上传点光源 ---
		int pointLightIndex = 0;
		for (auto& pl : s_3DSceneData.PoiLights) {
			if (pointLightIndex >= 16) break; // Shader 中最大定义为 16

			std::string baseName = "u_PointLights[" + std::to_string(pointLightIndex) + "]";

			shader->SetFloat3(baseName + ".position", pl.position);
			shader->SetFloat3(baseName + ".color", pl.color);
			shader->SetFloat(baseName + ".constant", pl.constant);
			shader->SetFloat(baseName + ".linear", pl.linear);
			shader->SetFloat(baseName + ".quadratic", pl.quadratic);
			shader->SetFloat(baseName + ".ambient", pl.ambient);
			shader->SetFloat(baseName + ".diffuse", pl.diffuse);
			shader->SetFloat(baseName + ".specular", pl.specular);

			pointLightIndex++;
		}
		// 告诉 Shader 实际有多少个点光源
		shader->SetInt("u_PointLightCount", pointLightIndex);
	}

}