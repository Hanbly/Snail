#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Camera/Camera.h"
#include "Camera/EditorCamera.h"
#include "Material/Material.h"

#include "Snail/Render/Renderer/Mesh/Mesh.h"
#include "Snail/Render/Renderer/Model/Model.h"

#include "Snail/Render/Renderer/RendererCommand.h"

namespace Snail {

	class Renderer3D
	{
	private:

		struct InstanceData {
			glm::mat4 ModelMatrix;
			glm::mat3 NormalMatrix;
		};

		// 3D 场景所需的全局数据
		struct Renderer3DSceneData {
			std::map<Refptr<Mesh>, std::vector<glm::mat4>> MeshInstanceQueue;	// 用于批量渲染
			Refptr<VertexBuffer> InstanceVBO;									// 实例的额外vbo

			glm::mat4 ViewProjectionMatrix; // VP 矩阵
			glm::vec3 CameraPosition;       // 相机位置 (用于镜面光计算)

			// 简单的单光源数据 (未来可以扩展为光源列表)
			glm::vec3 LightPosition;
			glm::vec4 LightColor;
			float AmbientStrength;
		};

		static Renderer3DSceneData s_3DSceneData;
		static const uint32_t MAX_INSTANCES_PER_BATCH = 10000; // 批量渲染限制

	public:
		static void Init();
		static void Shutdown();

		// BeginScene 升级：接收相机 + 光源信息
		static void BeginScene(const Camera* camera, const glm::mat4& transform, const glm::vec3& lightPos, const glm::vec4& lightColor, const float& ambient);
		static void EndScene();

		static void DrawSkybox(const Model& model, const EditorCamera& camera);
		static void DrawMesh(const Mesh& mesh, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawModel(const Model& model, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitMesh(const Refptr<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f));
		static void FlushMeshes();
	};

}