#pragma once

#include "Panel.h"

namespace Snail {

    // ==========================================================
    // 全局设置 (Environment & Stats)
    // ==========================================================
    class GlobalSettingsPanel : public Panel {
    private:
        Scene* m_Scene;
    public:
        GlobalSettingsPanel(Scene* scene)
            : m_Scene(scene) {}

        void SetScene(Scene* scene)
        {
            m_Scene = scene;
        }


        void Show()
        {            
            ImGui::Begin(u8"全局设置");

            // --- 性能 (Stats) ---
            ImGui::Text(u8"性能监控:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Separator();

            // --- 相机设置 (Camera) ---
            // 编辑器状态下，这里显示的可能是 EditorCamera 的信息
            // 运行时状态下，显示的可能是 Main Camera Entity 的信息
            // 这里暂时略过，因为通常这部分信息放在编辑器相机的 Component Inspector 或者专门的 Overlay 中更合适
            /*if (ImGui::TreeNodeEx(u8"相机设置", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (m_CameraEntity && m_CameraEntity.HasAllofComponent<TransformComponent>())
                {
                    auto& tc = m_CameraEntity.GetComponent<TransformComponent>();

                    glm::vec3 pos = tc.position;
                    glm::vec3 rot = tc.rotation;

                    ImGui::Text(u8"位置: %.2f,  %.2f,  %.2f", pos.x, pos.y, pos.z);
                    ImGui::Text(u8"旋角: %.2f°, %.2f°, %.2f°", rot.x, rot.y, rot.z);
                }
                ImGui::TreePop();
            }*/

            // --- 光源设置 (Lighting) ---
            if (m_Scene)
            {
                if (ImGui::TreeNodeEx(u8"场景光照", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    // 获取引用的方式修改 Scene 中的数据
                    auto& lightPos = m_Scene->GetLightPosition();
                    auto& lightColor = m_Scene->GetLightColor();

                    ImGui::DragFloat3(u8"光源位置", glm::value_ptr(lightPos), 0.5f);
                    ImGui::ColorEdit4(u8"光源颜色", glm::value_ptr(lightColor));

                    ImGui::TreePop();
                }
            }

            ImGui::End();
        }
    };

}