#pragma once

#include "Panel.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "glm/gtc/type_ptr.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"

namespace Snail {

    // ==========================================================
    // 场景（的实体）列表 (Scene Hierarchy) & 属性面板 (Properties)
    // ==========================================================
    class SceneHierarchyPanel : public Panel {
    private:
        Refptr<Scene> m_Scene;
        Entity m_SelectedEntity = {};
    public:
        SceneHierarchyPanel(const Refptr<Scene>& scene)
            : m_Scene(scene) {}

        void SetScene(const Refptr<Scene>& scene)
        {
            m_Scene = scene;
        }

        void Show()
        {
            if (!m_Scene) { SNL_CORE_ERROR("SceneHierarchyPanel: 未定义的场景信息!"); return; };

            ImGui::Begin(u8"场景列表 (Scene Hierarchy)");

            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            if (ImGui::Button("Add Entity"))
                ImGui::OpenPopup("AddEntity");

            if (ImGui::BeginPopup("AddEntity"))
            {                
                m_Scene->CreateEntity();

                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            ImGui::PopItemWidth();

            // 遍历所有对象（反向遍历保证顺序相同，正向遍历顺序会颠倒，这是entt的特性
            auto view = m_Scene->GetRegistry().view<entt::entity>();
            for (auto it = view.rbegin(), last = view.rend(); it != last; ++it) {
                Entity entity{ *it, m_Scene.get() };

                // 获取名字
                std::string name = "Unnamed Entity";
                if (entity.HasAllofComponent<TagComponent>())
                    name = entity.GetComponent<TagComponent>().name;

                ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
                flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

                bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(*it), flags, name.c_str());

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
                DrawComponents(m_SelectedEntity);
            }

            ImGui::End();
        }
    public:
        //const Entity& GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(const Entity& selectedEntity) { m_SelectedEntity = selectedEntity; }

    private: // -------------------- UI ------------------------
        void DrawEntityNode(Entity entity)
        {
            // 获取名字，如果没有 Tag 组件则给默认值
            std::string name = "Unnamed Entity";
            if (entity.HasAllofComponent<TagComponent>())
                name = entity.GetComponent<TagComponent>().name;

            ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

            // 使用 ID 指针作为唯一标识
            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

            if (ImGui::IsItemClicked())
            {
                m_SelectedEntity = entity;
            }

            if (opened) ImGui::TreePop();
        }

        void DrawComponents(Entity entity)
        {
            // --- Tag 组件 (特殊处理，不用折叠栏，放在最顶上) ---
            if (entity.HasAllofComponent<TagComponent>())
            {
                auto& tag = entity.GetComponent<TagComponent>().name;
                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strcpy_s(buffer, sizeof(buffer), tag.c_str());
                // 双倍宽度
                if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
                {
                    tag = std::string(buffer);
                }
            }

            ImGui::PushItemWidth(-1);
            if (ImGui::Button("Add Component"))
                ImGui::OpenPopup("AddComponent");
            if (ImGui::BeginPopup("AddComponent"))
            {
                if (ImGui::MenuItem("Model 现不支持添加，因为model没有数据")) {
                    // TODO: 现不支持添加，因为model没有数据
                    if (!m_SelectedEntity.HasAllofComponent<ModelComponent>())
                        //m_SelectedEntity.AddComponent<ModelComponent>();
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Point Light")) {
                    if (!m_SelectedEntity.HasAllofComponent<PointLightComponent>())
                        m_SelectedEntity.AddComponent<PointLightComponent>();
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete Entity"))
                ImGui::OpenPopup("DeleteEntity");

            if (ImGui::BeginPopup("DeleteEntity"))
            {
                m_Scene->DestroyEntity(entity);

                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return;
            }
            ImGui::PopItemWidth();

            // --- UUID 组件 (只读) ---
            DrawComponent<UUIDComponent>("UUID", entity, [](auto& component) {
                std::string uuidStr = (std::string)component; // 调用 operator std::string
                ImGui::TextDisabled("%s", uuidStr.c_str());
                }
            );

            // --- Transform 组件 ---
            DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
                DrawVec3Control("Position", component.position);
                DrawVec3Control("Rotation", component.rotation);
                DrawVec3Control("Scale", component.scale, 1.0f);
                }
            );

            // --- Point Light 组件 ---
            DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component) {
                ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
                ImGui::DragFloat("Intensity", &component.intensity, 0.1f, 0.0f, 100.0f);
                }
            );

			// --- Model 组件 ---
			if (entity.HasAllofComponent<SkyboxComponent, ModelComponent>()) {
				DrawComponent<ModelComponent>("Skybox", entity, [](auto& component) {
					ImGui::Checkbox("Visible", &component.visible);
					}
				);
			}
			else if (entity.HasAllofComponent<TransformComponent, ModelComponent>()) {
				DrawComponent<ModelComponent>("Mesh Renderer", entity, [](auto& component) {
					// 基础开关
					ImGui::Checkbox("Visible", &component.visible);
					ImGui::SameLine();
					ImGui::Checkbox("Show Outline", &component.edgeEnable);

					// Shader 区域 (使用灰色文本显示，暗示其重要但目前不可改)
					// (TODO: 未来添加修改 shader 功能)
					ImGui::TextDisabled("Shader:");
					ImGui::SameLine();
					ImGui::Text("%s", component.model->GetShaderPath().c_str());

					ImGui::Separator();

					// 模型来源信息
					bool isImported = component.model->IsImported();
					if (isImported) {
						ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "来源: 外部导入");
						ImGui::TextWrapped("Path: %s", component.model->GetFullPath().c_str());
					}
					else {
						ImGui::TextColored({ 1.0f, 0.8f, 0.4f, 1.0f }, "来源: 内部图元");
					}

					ImGui::Spacing();

					// Mesh 数据
					if (ImGui::TreeNodeEx("网格 & 纹理", ImGuiTreeNodeFlags_DefaultOpen)) {
						auto& meshes = component.model->GetMeshes();
						for (size_t m = 0; m < meshes.size(); m++) {
							auto& mesh = meshes[m];
							std::string meshLabel = "Mesh " + std::to_string(m) + " (" + PrimitiveTypeToString(mesh->GetPrimitiveType()) + ")";

							if (ImGui::TreeNode(meshLabel.c_str())) {
								auto textures = mesh->GetTextures();
								for (size_t t = 0; t < textures.size(); t++) {
									const auto& texture = textures[t];
									ImGui::PushID(t); // 防止同名 UI 冲突

									// 布局：左侧图片，右侧参数
									ImGui::BeginGroup();

									// 绘制图片
									uint32_t textureId = texture->GetRendererId();
									ImGui::Image((void*)(intptr_t)textureId, { 64.0f, 64.0f }, ImVec2(0, 1), ImVec2(1, 0), { 1,1,1,1 }, { 1,1,1,0.5f });

									ImGui::SameLine();

									// 绘制右侧文本区域
									ImGui::BeginGroup();
									ImGui::Text("Type:  %s", TextureTypeToString(texture->GetType()).c_str());
									ImGui::Text("Usage: %s", TextureUsageToString(texture->GetUsage()).c_str());

									// 路径折叠 (因为路径通常很长)
									if (ImGui::TreeNodeEx("Paths", ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
										auto& paths = texture->GetPath();
										for (const auto& p : paths) {
											ImGui::TextWrapped("- %s", p.c_str());
										}
									}
									ImGui::EndGroup();

									ImGui::EndGroup();

									if (t < textures.size() - 1) ImGui::Separator(); // 纹理间分割线
									ImGui::PopID();
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					}
                );
			}
        }

        static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
        {
            ImGui::PushID(label.c_str()); // 防止 ID 冲突

            // 两栏布局：左边是 Label，右边是控件
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text(label.c_str());
            ImGui::NextColumn();

            // 计算每个 float 控件的宽度
            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            // --- X Axis (红色) ---
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            if (ImGui::Button("X", buttonSize)) values.x = resetValue; // 点击重置
            ImGui::PopStyleColor(2);

            ImGui::SameLine();
            ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // --- Y Axis (绿色) ---
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
            ImGui::PopStyleColor(2);

            ImGui::SameLine();
            ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            // --- Z Axis (蓝色) ---
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
            ImGui::PopStyleColor(2);

            ImGui::SameLine();
            ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();
            ImGui::Columns(1); // 恢复
            ImGui::PopID();
        }

        template<typename T, typename UIFunction>
        static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
        {
            const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;

            if (entity.HasAllofComponent<T>())
            {
                // 获取组件引用
                auto& component = entity.GetComponent<T>();
                ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

                // 绘制 Header 样式
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
                float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

                ImGui::Separator();
                bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
                ImGui::PopStyleVar();

                // 右上角 "..." 按钮 (或者右键菜单)
                ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
                if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
                {
                    ImGui::OpenPopup("ComponentSettings");
                }

                bool removeComponent = false;
                if (ImGui::BeginPopup("ComponentSettings"))
                {
                    if (ImGui::MenuItem("Remove Component"))
                        removeComponent = true;
                    ImGui::EndPopup();
                }

                if (open)
                {
                    // 调用具体的绘制逻辑
                    uiFunction(component);
                    ImGui::TreePop();
                }

                if (removeComponent)
                    entity.RemoveComponent<T>();
            }
        }
    };

}