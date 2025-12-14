#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#include "Snail/Scene/Entity.h"
#include "Snail/Scene/Component.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Snail {

    struct MouseRay {
        glm::vec3 origin;    // 射线起点
        glm::vec3 direction; // 射线方向（单位向量）

        MouseRay(const float& x, const float& y, const float& width, const float& height, const glm::mat4& view, const glm::mat4& proj)
        {
            // 映射为 NDC 屏幕坐标：-1 ~ +1，经过矩阵转置的坐标结果就是NDC，现在要进行逆向操作
            float ndcX = (2.0f * x) / (float)width - 1.0f;
            float ndcY = 1.0f - (2.0f * y) / (float)height; // TODO: OpenGL的y轴反转，这里先这样写

            origin = origin = glm::vec3(glm::inverse(view)[3]);
            // 必须 矩阵 * 向量
            glm::vec4 rayEye = glm::inverse(proj) * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
            // 这一步决定了是在算“方向”
            rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
            glm::vec3 rayWorld = glm::vec3(glm::inverse(view) * rayEye);
            // 归一化
            direction = glm::normalize(rayWorld);
        }

        bool Is_Cross(Entity& entity, float& outDistance)
        {
            glm::vec3 localMin(std::numeric_limits<float>::max());
            glm::vec3 localMax(std::numeric_limits<float>::lowest());

            bool HasGeometry = false;

            if (entity.HasAllofComponent<ModelComponent>()) {
                const auto& model = entity.GetComponent<ModelComponent>().model;

                if (!model->GetMeshs().empty()) {
                    localMin = model->m_AABB.min;
                    localMax = model->m_AABB.max;
                    HasGeometry = true;
                }
                
            }

            if (!HasGeometry) return false;

            // 2. 将局部 AABB 的 8 个角点变换到世界空间
            glm::mat4 transform = entity.GetComponent<TransformComponent>().GetTransform();

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
