#pragma once

#include "Panel.h"

namespace Snail {

    // ==========================================================
    // 全局设置 (Environment & Stats)
    // ==========================================================
    class GlobalSettingsPanel : public Panel {
    private:
        Refptr<Scene> m_Scene;
    public:
        GlobalSettingsPanel(const Refptr<Scene>& scene)
            : m_Scene(scene) {}

        void SetScene(const Refptr<Scene>& scene)
        {
            m_Scene = scene;
        }


        void Show(const Refptr<EditorCamera>& ec)
        {            
            ImGui::Begin(u8"全局设置");

            // --- 性能 (Stats) ---
            ImGui::Text(u8"性能监控:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Separator();

            // --- 相机设置 (Camera) ---
            if (ImGui::TreeNodeEx(u8"相机设置", ImGuiTreeNodeFlags_DefaultOpen))
            {                
                glm::vec3 pos = ec->GetPostion();
                glm::vec3 rot = ec->GetRotation();

                ImGui::Text(u8"位置: %.2f,  %.2f,  %.2f", pos.x, pos.y, pos.z);
                ImGui::Text(u8"旋角: %.2f°, %.2f°, %.2f°", rot.x, rot.y, rot.z);

                const char* modeTypeStrings[] = { "None", "自由漫游（FPS）模式", "轨道球（Arcball）模式" };
                const char* currentModeString = modeTypeStrings[(int)ec->GetMode()];

                if (ImGui::BeginCombo(u8"控制模式", currentModeString))
                {
                    for (int i = 0; i < 3; i++) // 遍历所有模式
                    {
                        bool isSelected = (currentModeString == modeTypeStrings[i]);
                        if (ImGui::Selectable(modeTypeStrings[i], isSelected))
                        {
                            currentModeString = modeTypeStrings[i];
                            ec->SetMode((EditorCameraMode)i);
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::TreePop();
            }

            // --- 光源设置 (Lighting) ---
            if (m_Scene)
            {
                if (ImGui::TreeNodeEx(u8"场景光照", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    // 获取引用的方式修改 Scene 中的数据
                    auto& ambient = m_Scene->GetAmbientStrength();
                    
                    ImGui::DragFloat("Ambient", &ambient, 0.001f, 0.0f, 1.0f, "%.2f");

                    ImGui::TreePop();
                }
            }

            ImGui::End();
        }
    };

}