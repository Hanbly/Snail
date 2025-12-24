
#include "SceneHierarchyPanel.h"

#include <glm/gtc/type_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Snail{

	void SceneHierarchyPanel::Show()
	{
		if (!m_Scene) { SNL_CORE_ERROR("SceneHierarchyPanel: 未定义的场景信息!"); return; };

		ImGui::Begin(u8"场景列表");

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 4.0f });
		auto view = m_Scene->GetRegistry().view<entt::entity>();
		for (auto it = view.rbegin(), last = view.rend(); it != last; ++it) {
			Entity entity{ *it, m_Scene.get() };

			std::string name = "Unnamed Entity";
			if (entity.HasAllofComponent<TagComponent>())
				name = entity.GetComponent<TagComponent>().name;

			ImGuiTreeNodeFlags flags = ((m_Context->selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

			bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(*it), flags, name.c_str());

			if (ImGui::IsItemClicked())
			{
				ResetSelectedEntity(entity);
			}

			if (opened) ImGui::TreePop();
		}
		ImGui::PopStyleVar();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushItemWidth(-1);
		if (ImGui::Button("添加实体", { -1, 30 }))
			ImGui::OpenPopup("AddEntity");

		if (ImGui::BeginPopup("AddEntity"))
		{
			m_Scene->CreateEntity();

			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();

		ImGui::End();

		ImGui::Begin(u8"属性面板");

		if (m_Context->selectedEntity)
		{
			DrawComponents(m_Context->selectedEntity);
		}

		ImGui::End();

		FileSelecter::Handle("ModelImportKey", [this](const std::string& path) {
			if (m_Context->selectedEntity) {
				auto shader = ShaderLibrary::Load("assets/shaders/Standard.glsl");
				auto model = std::make_shared<Model>(shader, path);
				m_Context->selectedEntity.AddComponent<ModelComponent>(model);
			}
			});
		FileSelecter::Handle("EditTexture2D", [this](const std::string& path) {
			auto entity = m_Context->selectedEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();
			size_t mIdx = m_Context->currentEditingMeshIndex;
			size_t tIdx = m_Context->currentEditingTexIndex;

			if (mIdx < meshes.size()) {
				auto& mesh = meshes[mIdx];

				mesh->EditTexture(tIdx, path);
			}
			});
		FileSelecter::Handle("AddNewTexture", [this](const std::string& path) {
			auto entity = m_Context->selectedEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();
			size_t mIdx = m_Context->currentEditingMeshIndex;

			if (mIdx < meshes.size()) {
				auto& mesh = meshes[mIdx];

				TextureUsage targetUsage = m_Context->pendingTextureUsage;

				if (auto newTexture = TextureLibrary::Load({ path }, targetUsage)) {
					mesh->AddTexture(newTexture, targetUsage);
				}
			}
			});
	}

	void SceneHierarchyPanel::ResetSelectedEntity(const Entity& entity) {
		auto modelview = m_Scene->GetAllofEntitiesWith<ModelComponent>();
		for (auto [entity, model] : modelview.each())
			model.edgeEnable = false;

		m_Context->selectedEntity = entity;

		if (m_Context->selectedEntity && m_Context->selectedEntity.IsValid()) {
			if (m_Context->selectedEntity.HasAllofComponent<ModelComponent>()) {
				m_Context->selectedEntity.GetComponent<ModelComponent>().edgeEnable = true;
			}
		}
	}
	void SceneHierarchyPanel::AddSelectedEntity(const Entity& entity) {
		if (m_Context->selectedEntity == entity) return;

		m_Context->selectedEntity = {};

		if (entity && entity.IsValid()) {
			if (entity.HasAllofComponent<ModelComponent>()) {
				entity.GetComponent<ModelComponent>().edgeEnable = true;
			}
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		std::string name = "UnnamedEntity";
		if (entity.HasAllofComponent<TagComponent>())
			name = entity.GetComponent<TagComponent>().name;

		ImGuiTreeNodeFlags flags = ((m_Context->selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			m_Context->selectedEntity = entity;
		}

		if (opened) ImGui::TreePop();
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasAllofComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().name;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::BeginTable("TagTable", 2, ImGuiTableFlags_SizingFixedFit)) {
				ImGui::TableSetupColumn("Icon", ImGuiTableColumnFlags_WidthFixed, 34.0f);
				ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::TextDisabled("标签");
				ImGui::TableSetColumnIndex(1);
				if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
				ImGui::EndTable();
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		ImGui::Spacing();
		if (ImGui::Button("添加组件", { -1, 24 }))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::BeginMenu("模型组件"))
			{
				if (ImGui::BeginMenu("基础图元"))
				{
					auto AddPrimitive = [&](PrimitiveType type, const std::string& name) {
						if (!m_Context->selectedEntity.HasAllofComponent<ModelComponent>() && type != PrimitiveType::Skybox) {
							auto defaultShader = ShaderLibrary::Load("assets/shaders/Standard.glsl");
							auto model = std::make_shared<Model>(type, defaultShader, std::vector<Refptr<Texture>>{});
							m_Context->selectedEntity.AddComponent<ModelComponent>(model);
						}
						else if (!m_Context->selectedEntity.HasAllofComponent<ModelComponent>() && type == PrimitiveType::Skybox) {
							auto skyShader = ShaderLibrary::Load("assets/shaders/TextureCube_Shader.glsl");
							std::vector<Refptr<Texture>> textureDataList;
							std::vector<std::string> paths = {
								"assets/images/defaultSky/right.jpg",
								"assets/images/defaultSky/left.jpg",
								"assets/images/defaultSky/top.jpg",
								"assets/images/defaultSky/bottom.jpg",
								"assets/images/defaultSky/front.jpg",
								"assets/images/defaultSky/back.jpg"
							};
							if (auto texture = TextureLibrary::Load("DefaultSkyboxTextures", paths, TextureUsage::Cubemap)) {
								textureDataList.push_back(texture);
								auto model = std::make_shared<Model>(type, skyShader, textureDataList);
								m_Context->selectedEntity.AddComponent<ModelComponent>(model);
								m_Context->selectedEntity.RemoveComponent<TransformComponent>();
							}
						}
						ImGui::CloseCurrentPopup();
						};

					if (ImGui::MenuItem("Cube"))   AddPrimitive(PrimitiveType::Cube, "Cube");
					if (ImGui::MenuItem("Sphere")) AddPrimitive(PrimitiveType::Sphere, "Sphere");
					if (ImGui::MenuItem("Plane"))  AddPrimitive(PrimitiveType::Plane, "Plane");
					if (ImGui::MenuItem("Skybox"))  AddPrimitive(PrimitiveType::Skybox, "Skybox");

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("导入外部模型..."))
				{
					FileSelecter::Open("ModelImportKey", "导入模型", "(.obj,.fbx,.gltf){.obj,.fbx,.gltf},.obj,.fbx,.gltf");
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		if (ImGui::Button("删除该实体", { -1, 24 }))
			ImGui::OpenPopup("DeleteEntity");
		ImGui::PopStyleColor(2);

		bool deleteFlag = false;
		if (ImGui::BeginPopup("DeleteEntity"))
		{
			m_Scene->DestroyEntity(entity);

			auto modelview = m_Scene->GetAllofEntitiesWith<TransformComponent, ModelComponent>();
			for (auto [e, transform, model] : modelview.each())
				if (model.edgeEnable == true)
					m_Scene->DestroyEntity({ e, m_Scene.get() });

			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();

			deleteFlag = true;
		}
		ImGui::PopItemWidth();
		if (deleteFlag) return;

		DrawComponent<UUIDComponent>("UUID", entity, [](auto& component) {
			std::string uuidStr = (std::string)component;
			if (ImGui::BeginTable("UUIDTable", 2, ImGuiTableFlags_BordersInnerV)) {
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Value");
				ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("%s", uuidStr.c_str());
				ImGui::EndTable();
			}
			}
		);

		DrawComponent<TransformComponent>("Transform", entity, [this](auto& component) {
			DrawVec3Control("Position", component.position);
			DrawVec3Control("Rotation", component.rotation);
			DrawVec3Control("Scale", component.scale, 1.0f);
			}
		);

		DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component) {
			if (ImGui::BeginTable("LightProps", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Control");

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Color");
				ImGui::TableSetColumnIndex(1); ImGui::ColorEdit4("##Color", glm::value_ptr(component.color), ImGuiColorEditFlags_NoInputs);

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Intensity");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Intensity", &component.intensity, 0.1f, 0.0f, 100.0f);

				ImGui::EndTable();
			}
			}
		);

		if (entity.HasAllofComponent<ModelComponent>()) {
			DrawComponent<ModelComponent>("渲染网格", entity, [this](auto& component) {
				if (ImGui::BeginTable("ModelSettings", 2, ImGuiTableFlags_BordersInnerV)) {
					ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
					ImGui::TableSetupColumn("Value");

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("Visible");
					ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Visible", &component.visible);

					if (component.model->GetPrimitiveType() != PrimitiveType::Skybox) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0); ImGui::Text("Outline");
						ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Outline", &component.edgeEnable);
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::TextDisabled("Shader");
					ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("%s", component.model->GetShaderPath().c_str());
					ImGui::EndTable();
				}

				ImGui::Separator();

				bool isImported = component.model->IsImported();
				if (isImported) {
					ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "Source: Imported");
					ImGui::TextWrapped("Path: %s", component.model->GetFullPath().c_str());
				}
				else {
					ImGui::TextColored({ 1.0f, 0.8f, 0.4f, 1.0f }, "Source: Primitive");
				}

				ImGui::Spacing();

				if (ImGui::TreeNodeEx("Meshes & Textures", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
					auto& meshes = component.model->GetMeshes();
					for (size_t m = 0; m < meshes.size(); m++) {
						auto& mesh = meshes[m];
						// 如果没有纹理，则不添加 DefaultOpen 标志
						ImGuiTreeNodeFlags meshFlags = 0;
						if (!mesh->GetTextures().empty()) {
							meshFlags |= ImGuiTreeNodeFlags_DefaultOpen;
						}
						std::string meshLabel = "Mesh " + std::to_string(m) + " (" + PrimitiveTypeToString(mesh->GetPrimitiveType()) + ")";

						if (ImGui::TreeNodeEx(meshLabel.c_str(), meshFlags)) {
							auto textures = mesh->GetTextures();
							int textureIndexToRemove = -1;

							if (ImGui::BeginTable("TextureSlots", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
								ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, 64.0f);
								ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
								ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 40.0f);

								for (size_t t = 0; t < textures.size(); t++) {
									auto& texture = textures[t];
									ImGui::PushID(static_cast<int>(t));

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									uint32_t textureId = texture->GetRendererId();
									if (ImGui::ImageButton("##texBtn", (void*)(intptr_t)textureId, { 56.0f, 56.0f }, { 0, 1 }, { 1, 0 })) {
										m_Context->currentEditingMeshIndex = m;
										m_Context->currentEditingTexIndex = t;
										FileSelecter::Open("EditTexture2D", "选择纹理", "(.png,.jpg,.jpeg,.tga,.bmp){.png,.jpg,.jpeg,.tga,.bmp},.png,.jpg,.jpeg,.tga,.bmp");
									}
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("点击更换纹理");

									ImGui::TableSetColumnIndex(1);
									ImGui::TextColored({ 0.8f, 0.8f, 0.8f, 1.0f }, "%s", TextureUsageToString(texture->GetUsage()).c_str());
									ImGui::TextDisabled("%s", TextureTypeToString(texture->GetType()).c_str());
									if (texture->GetPath().size() > 0)
										ImGui::TextWrapped("%s", texture->GetPath()[0].c_str());

									ImGui::TableSetColumnIndex(2);
									ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.4f));
									ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
									if (ImGui::Button("X", { 24, 24 })) {
										textureIndexToRemove = static_cast<int>(t);
									}
									ImGui::PopStyleColor(2);
									if (ImGui::IsItemHovered()) ImGui::SetTooltip("移除此纹理");

									ImGui::PopID();
								}
								ImGui::EndTable();
							}

							if (textureIndexToRemove != -1) {
								mesh->RemoveTexture(textureIndexToRemove);
							}

							ImGui::Spacing();
							if (ImGui::Button(" + 添加新纹理 ", { -1, 28 })) {
								ImGui::OpenPopup("SelectTextureUsagePopup");
							}

							if (ImGui::BeginPopup("SelectTextureUsagePopup"))
							{
								ImGui::TextDisabled("选择纹理用途");
								ImGui::Separator();

								auto AddTextureMenuItem = [&](const char* label, TextureUsage usage) {
									if (ImGui::MenuItem(label)) {
										m_Context->currentEditingMeshIndex = m;
										m_Context->pendingTextureUsage = usage;
										FileSelecter::Open("AddNewTexture", "导入新纹理", "(.png,.jpg,.jpeg,.tga,.bmp){.png,.jpg,.jpeg,.tga,.bmp},.png,.jpg,.jpeg,.tga,.bmp");
										ImGui::CloseCurrentPopup();
									}
									};

								AddTextureMenuItem("Diffuse / Albedo", TextureUsage::Diffuse);
								AddTextureMenuItem("Specular / Metallic", TextureUsage::Specular);
								AddTextureMenuItem("Cube Map", TextureUsage::Cubemap);

								ImGui::EndPopup();
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

	void SceneHierarchyPanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		if (ImGui::Button("X", buttonSize)) values.x = resetValue;
		ImGui::PopStyleColor(2);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
		ImGui::PopStyleColor(2);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
		ImGui::PopStyleColor(2);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	void SceneHierarchyPanel::DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasAllofComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

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
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

}