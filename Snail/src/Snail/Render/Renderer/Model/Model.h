#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Render/Renderer/Mesh/Mesh.h"
#include "Snail/Render/Renderer/Camera/PerspectiveCameraController.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Snail {

	class Model {
	private:
		Refptr<Shader> m_Shader;
		std::vector<Refptr<Mesh>> m_Meshs;
		std::vector<TextureData> m_LoadedTextures;
		std::string m_Directory;
	public:
		Model(const Refptr<Shader>& shader, const std::string& path);
		~Model() = default;

		inline const std::vector<Refptr<Mesh>>& GetMeshs() const { return m_Meshs; }

		void Draw(const glm::mat4& worldTransform, const bool& edgeEnable) const;

	private:
		void Load(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransformation);
		Refptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& localTransformation);
		std::vector<TextureData> LoadMaterialTextures(aiMaterial* mat, const aiTextureType& type, const std::string& typeName);

		//--------------Tools--------------------
		glm::mat4 ConvertaiMat4ToglmMat4(const aiMatrix4x4& matrix) const;
	};

    struct ModelInstance {
        std::string name;       // 在编辑器里显示的名字
        Refptr<Model> model;    // 指向的模型资源
        Refptr<Mesh> mesh;      // 兼容单一 Mesh
        bool edgeEnable = false;

        // 变换参数
        glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
        glm::vec3 rot = { 0.0f, 0.0f, 0.0f };
        glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

        // 材质参数 (每个物体独立)
        float ambient = 0.1f;
        float diffuse = 0.8f;
        float specular = 0.5f;
        float shininess = 32.0f;

        // 获取变换矩阵
        glm::mat4 GetTransform() const {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, pos);
            transform = glm::rotate(transform, glm::radians(rot.x), { 1,0,0 });
            transform = glm::rotate(transform, glm::radians(rot.y), { 0,1,0 });
            transform = glm::rotate(transform, glm::radians(rot.z), { 0,0,1 });
            transform = glm::scale(transform, scale);
            return transform;
        }
    };

    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    struct MouseRay {
        glm::vec3 origin;    // 射线起点
        glm::vec3 direction; // 射线方向（单位向量）

        MouseRay(const float& x, const float& y, const float& width, const float& height, const Refptr<PerspectiveCameraController>& cameraController)
        {
            // 映射为 NDC 屏幕坐标：-1 ~ +1，经过矩阵转置的坐标结果就是NDC，现在要进行逆向操作
            float ndcX = (2.0f * x) / (float)width - 1.0f;
            float ndcY = 1.0f - (2.0f * y) / (float)height; // TODO: OpenGL的y轴反转，这里先这样写

            origin = cameraController->GetCamera()->GetCameraPos();
            // 必须 矩阵 * 向量
            glm::vec4 rayEye = glm::inverse(cameraController->GetCamera()->GetProjectionMatrix()) * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
            // 这一步决定了是在算“方向”
            rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
            glm::vec3 rayWorld = glm::vec3(glm::inverse(cameraController->GetCamera()->GetViewMatrix()) * rayEye);
            // 归一化
            direction = glm::normalize(rayWorld);
        }

        bool Is_Cross(const Refptr<ModelInstance>& model, float& outDistance)
        {
            // 1. 获取模型局部包围盒 (前提：你已经在 Mesh 加载时算好了 m_MinVertex 和 m_MaxVertex)
            // 如果 ModelInstance 有多个 Mesh，你需要遍历合并它们的包围盒
            // 这里假设 model->mesh 是主 Mesh
            glm::vec3 localMin(std::numeric_limits<float>::max());
            glm::vec3 localMax(std::numeric_limits<float>::lowest());
            if (model->mesh) {
                localMin = model->mesh->m_MinVertex;
                localMax = model->mesh->m_MaxVertex;
            }
            else if (model->model) {
                const auto& meshes = model->model->GetMeshs();
                if (!meshes.empty()) {
                    // 先以第一个网格作为基准
                    localMin = meshes[0]->m_MinVertex;
                    localMax = meshes[0]->m_MaxVertex;
                    for (size_t i = 1; i < meshes.size(); i++) {
                        localMin = glm::min(localMin, meshes[i]->m_MinVertex);
                        localMax = glm::max(localMax, meshes[i]->m_MaxVertex);
                    }
                }
            }

            // 2. 将局部 AABB 的 8 个角点变换到世界空间
            glm::mat4 transform = model->GetTransform();

            glm::vec3 corners[8];
            corners[0] = glm::vec3(localMin.x, localMin.y, localMin.z);
            corners[1] = glm::vec3(localMin.x, localMin.y, localMax.z);
            corners[2] = glm::vec3(localMin.x, localMax.y, localMin.z);
            corners[3] = glm::vec3(localMin.x, localMax.y, localMax.z);
            corners[4] = glm::vec3(localMax.x, localMin.y, localMin.z);
            corners[5] = glm::vec3(localMax.x, localMin.y, localMax.z);
            corners[6] = glm::vec3(localMax.x, localMax.y, localMin.z);
            corners[7] = glm::vec3(localMax.x, localMax.y, localMax.z);

            glm::vec3 worldMin(std::numeric_limits<float>::max());
            glm::vec3 worldMax(std::numeric_limits<float>::lowest());

            for (int i = 0; i < 8; i++) {
                // 变换点
                glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
                glm::vec3 p = glm::vec3(transformed); // / transformed.w; // 通常 w=1

                // 重新寻找新的 AABB 边界
                worldMin = glm::min(worldMin, p);
                worldMax = glm::max(worldMax, p);
            }

            // 3. Slab Method 检测算法
            float tmin = (worldMin.x - origin.x) / direction.x;
            float tmax = (worldMax.x - origin.x) / direction.x;

            if (tmin > tmax) std::swap(tmin, tmax);

            float tymin = (worldMin.y - origin.y) / direction.y;
            float tymax = (worldMax.y - origin.y) / direction.y;

            if (tymin > tymax) std::swap(tymin, tymax);

            if ((tmin > tymax) || (tymin > tmax))
                return false;

            if (tymin > tmin) tmin = tymin;
            if (tymax < tmax) tmax = tymax;

            float tzmin = (worldMin.z - origin.z) / direction.z;
            float tzmax = (worldMax.z - origin.z) / direction.z;

            if (tzmin > tzmax) std::swap(tzmin, tzmax);

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;

            if (tzmin > tmin) tmin = tzmin;
            if (tzmax < tmax) tmax = tzmax;

            // tmin 是相交点距离相机的距离
            outDistance = tmin;

            // 如果 tmin < 0，说明物体在相机背后，但在射线直线上
            if (outDistance < 0) return false;

            return true;
        }
    };

}