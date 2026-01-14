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
		glm::vec3 color;

		float intensity;
		DirectionLight(const glm::vec3& dir, const glm::vec3& color, const float& intensity)
			:
			direction(dir),
			color(color),
			intensity(intensity)
		{}
	};

	// --- 点光源数据 ---
	struct PointLight {
		glm::vec3 position;
		glm::vec3 color;
		float intensity;

		//float constant;
		//float linear;
		//float quadratic;
		PointLight(const glm::vec3& pos, const glm::vec3& color, const float& intensity)
			:
			position(pos),
			color(color),
			intensity(intensity)
		{}
	};

	class Renderer3D
	{
	private:

		struct InstanceData {
			glm::mat4 ModelMatrix;
			glm::mat3 NormalMatrix;
			int EntityID; // 用于 MRT 输出物体轮廓

			float Padding[2];
		};

		struct MeshRenderData {
			glm::mat4 Transform;
			int EntityID;
		};

		// 3D 场景所需的全局数据
		struct Renderer3DSceneData {
			bool EnableInstancing = false;												// 是否批量渲染
			std::map<Refptr<Mesh>, std::vector<MeshRenderData>> MeshInstanceQueue;	// 用于批量渲染
			Refptr<VertexBuffer> InstanceVBO;									// 实例的额外vbo

			glm::mat4 ViewProjectionMatrix; // VP 矩阵
			glm::vec3 CameraPosition;       // 相机位置 (用于镜面光计算)

			// 光源列表
			std::vector<DirectionLight> DirLights;
			std::vector<PointLight> PoiLights;

			// 光源空间矩阵 和 光源视角的深度贴图id
			glm::mat4 MainLightSpace = glm::mat4(1.0f);
			uint32_t ShadowRendererId = 0;

			// --- IBL 数据 ---
			bool UseIBL = true;
			Refptr<Texture> IrradianceMap = nullptr;
			Refptr<Texture> PrefilterMap = nullptr;
			Refptr<Texture> BRDFLUT = nullptr; // BRDF LUT 通常是全局唯一的资源

			const int ShadowMapSlot = 10;
			const int IBLMapsStartSlot = 11;
		};

		static Renderer3DSceneData s_3DSceneData;
		static const uint32_t MAX_INSTANCES_PER_BATCH = 50000; // 批量渲染限制

	public:
		static void Init();
		static void Shutdown();

		// BeginScene 升级：接收相机 + 光源信息
		static void BeginScene(const Camera* camera, const glm::mat4& transform, 
			std::vector<DirectionLight>& dirLights, std::vector<PointLight>& poiLights, 
			const glm::mat4& mainLightSpace, const uint32_t& shadowRendererId,
			const Refptr<Texture>& irradianceMap = nullptr,
			const Refptr<Texture>& prefilterMap = nullptr,
			const Refptr<Texture>& brdfLut = nullptr);
		static void EndScene();
		static void EndScene(Refptr<Shader>& shader);

		// --- 正常绘制流程的函数 ---
		static void DrawSkybox(const Model& model, const EditorCamera& camera);
		static void DrawMesh(const Mesh& mesh, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitMesh(const Refptr<Mesh>& mesh, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));
		static void FlushMeshes();
		static void DrawModel(const Model& model, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));

		// --- 接收外部shader绘制 --- （用于阴影贴图绘制等）
		static void DrawMesh(Refptr<Shader>& shader, const Mesh& mesh, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitMesh(const Refptr<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f));
		static void FlushMeshes(Refptr<Shader>& shader);
		static void DrawModel(Refptr<Shader>& shader, const Model& model, const glm::mat4& transform);

		static void SetEnableInstancing(const bool& status) { s_3DSceneData.EnableInstancing = status; }
		static bool& GetEnableInstancing() { return s_3DSceneData.EnableInstancing; }
		static void SetUseIBL(const bool& status) { s_3DSceneData.UseIBL = status; }
		static bool& GetUseIBL() { return s_3DSceneData.UseIBL; }
	private:
		static void UploadLightsUniforms(const Refptr<Shader>& shader);
		static void UploadEnvUniforms(const Refptr<Shader>& shader, int startSlot);
	};

}