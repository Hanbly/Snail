#pragma once

#include "Panel.h"

namespace Snail {

    // ==========================================================
    // 场景（的实体）列表 (Scene Hierarchy) & 属性面板 (Properties)
    // ==========================================================
    class SceneHierarchyPanel : public Panel {
    private:
        Scene* m_Scene;
        Entity m_SelectedEntity = {};
    public:
        SceneHierarchyPanel(Scene* scene)
            : m_Scene(scene) {}

        void SetScene(Scene* scene)
        {
            m_Scene = scene;
        }

        void Show()
        {
            if (!m_Scene) { SNL_CORE_ERROR("SceneHierarchyPanel: 未定义的场景信息!"); return; };

            ImGui::Begin(u8"场景列表 (Scene Hierarchy)");

            // 遍历所有对象
            auto view = m_Scene->GetAllofEntitiesWith<TransformComponent>();
            for (auto entityID : view) {
                Entity entity{ entityID, m_Scene };

                // 获取名字
                std::string name = "Unnamed Entity";
                if (entity.HasAllofComponent<TagComponent>())
                    name = entity.GetComponent<TagComponent>().name;

                ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
                flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

                bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entityID, flags, name.c_str());

                if (ImGui::IsItemClicked())
                {
                    // 处理切换选中：旧的取消描边，新的开启描边
                    if (m_SelectedEntity && m_SelectedEntity.HasAllofComponent<ModelComponent>())
                        m_SelectedEntity.GetComponent<ModelComponent>().edgeEnable = false;

                    m_SelectedEntity = entity;

                    if (m_SelectedEntity.HasAllofComponent<ModelComponent>())
                        m_SelectedEntity.GetComponent<ModelComponent>().edgeEnable = true;
                }

                if (opened) ImGui::TreePop();
            }

            ImGui::End();

            ImGui::Begin(u8"属性面板 (Properties)");

            if (m_SelectedEntity)
            {
                // --- Tag 组件 ---
                if (m_SelectedEntity.HasAllofComponent<TagComponent>())
                {
                    auto& tag = m_SelectedEntity.GetComponent<TagComponent>();
                    char buffer[256];
                    memset(buffer, 0, sizeof(buffer));
                    strcpy_s(buffer, sizeof(buffer), tag.name.c_str());
                    if (ImGui::InputText("Name", buffer, sizeof(buffer)))
                        tag.name = std::string(buffer);
                }

                // --- Transform 组件 ---
                if (m_SelectedEntity.HasAllofComponent<TransformComponent>())
                {
                    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();

                        glm::vec3 pos = transform.position;
                        if (ImGui::DragFloat3("位置", glm::value_ptr(pos))) {
                            transform.position = pos;
                        }
                        glm::vec3 rot = transform.rotation;
                        if (ImGui::DragFloat3("旋转", glm::value_ptr(rot))) {
                            transform.rotation = rot;
                        }
                        float sca = transform.scale.x;
                        if (ImGui::DragFloat("缩放", &sca)) {
                            transform.scale = glm::vec3(sca);
                        }
                    }
                }

                // --- 点光源 组件 ---
                if (m_SelectedEntity.HasAllofComponent<PointLightComponent>())
                {
                    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        auto& plc = m_SelectedEntity.GetComponent<PointLightComponent>();
                        ImGui::ColorEdit4("Color", glm::value_ptr(plc.color));
                        ImGui::DragFloat("Intensity", &plc.intensity, 0.1f);
                    }
                }

                // --- Model 组件 ---
                if (m_SelectedEntity.HasAllofComponent<ModelComponent>())
                {
                    if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        auto& mc = m_SelectedEntity.GetComponent<ModelComponent>();
                        ImGui::Checkbox("显示", &mc.visible);
                        ImGui::Checkbox("物体轮廓", &mc.edgeEnable);
                    }
                }
            }

            ImGui::End();
        }

        //const Entity& GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(const Entity& selectedEntity) { m_SelectedEntity = selectedEntity; }
    };

}