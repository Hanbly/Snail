#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Render/RenderAPI/VertexArray.h"
#include "Camera/Camera.h"
#include "Material/Material.h"

#include "Snail/Render/Renderer/Mesh/Mesh.h"
#include "Snail/Render/Renderer/Model/Model.h"

#include "Snail/Render/Renderer/RendererCommand.h"

namespace Snail {

	class Renderer3D
	{
	private:
		// 3D 场景所需的全局数据
		struct Renderer3DSceneData {
			glm::mat4 ViewProjectionMatrix; // VP 矩阵
			glm::vec3 CameraPosition;       // 相机位置 (用于镜面光计算)

			// 简单的单光源数据 (未来可以扩展为光源列表)
			glm::vec3 LightPosition;
			glm::vec4 LightColor;

			ShaderLibrary ShaderLibrary;
		};

		static Renderer3DSceneData s_3DSceneData;

	public:
		static void Init();
		static void Shutdown();

		// BeginScene 升级：接收相机 + 光源信息
		static void BeginScene(const Camera& camera, const glm::mat4& transform, const glm::vec3& lightPos, const glm::vec4& lightColor);
		static void EndScene();

		static void DrawMesh(const Mesh& mesh, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));
		static void DrawModel(const Model& model, const bool& edgeEnable, const glm::mat4& transform = glm::mat4(1.0f));
	};

}