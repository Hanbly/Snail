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

	// --- 平行光数据 ---
	struct DirectionLight {
		glm::vec3 direction;
		glm::vec4 color;

		float ambient;
		float diffuse;
		float specular;
		DirectionLight(const glm::vec3& dir, const glm::vec4& color, const float& ambient, const float& diffuse, const float& specular)
			:
			direction(dir),
			color(color),
			ambient(ambient), diffuse(diffuse), specular(specular)
		{}
	};

	// --- 点光源数据 ---
	struct PointLight {
		glm::vec3 position;
		glm::vec4 color;

		float constant;
		float linear;
		float quadratic;

		float ambient;
		float diffuse;
		float specular;
		PointLight(const glm::vec3& pos, const glm::vec4& color, const float& constant, const float& linear, const float& quadratic, const float& ambient, const float& diffuse, const float& specular)
			:
			position(pos),
			color(color),
			constant(constant), linear(linear), quadratic(quadratic),
			ambient(ambient), diffuse(diffuse), specular(specular)
		{}
	};

	class Renderer3D
	{
	private:

		struct InstanceData {
			glm::mat4 ModelMatrix;
			glm::mat3 NormalMatrix;
			int EntityID; // 用于 MRT 输出物体轮廓
		};

		struct MeshRenderData {
			glm::mat4 Transform;
			int EntityID;
		};

		// 3D 场景所需的全局数据
		struct Renderer3DSceneData {
			std::map<Refptr<Mesh>, std::vector<MeshRenderData>> MeshInstanceQueue;	// 用于批量渲染
			Refptr<VertexBuffer> InstanceVBO;									// 实例的额外vbo

			glm::mat4 ViewProjectionMatrix; // VP 矩阵
			glm::vec3 CameraPosition;       // 相机位置 (用于镜面光计算)

			// 光源列表
			std::vector<DirectionLight> DirLights;
			std::vector<PointLight> PoiLights;
		};

		static Renderer3DSceneData s_3DSceneData;
		static const uint32_t MAX_INSTANCES_PER_BATCH = 50000; // 批量渲染限制

	public:
		static void Init();
		static void Shutdown();

		// BeginScene 升级：接收相机 + 光源信息
		static void BeginScene(const Camera* camera, const glm::mat4& transform, std::vector<DirectionLight>& dirLights, std::vector<PointLight>& poiLights);
		static void EndScene();

		static void DrawSkybox(const Model& model, const EditorCamera& camera);
		static void DrawMesh(const Mesh& mesh, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawModel(const Model& model, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));

		static void SubmitMesh(const Refptr<Mesh>& mesh, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));
		static void FlushMeshes();
	private:
		static void UploadLightsUniforms(const Refptr<Material>& material);
	};

}