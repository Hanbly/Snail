#include "InspectorPanel.h"

#include "Snail/Utils/FileSelecter.h"

#include <glm/gtc/type_ptr.hpp>

namespace Snail {

	void InspectorPanel::Show()
	{
		ImGui::Begin(u8"属性面板");

		if (m_Context->selectedEntity && m_Context->selectedEntity.IsValid())
		{
			DrawAllComponents(m_Context->selectedEntity);
		}
		else
		{
			// 检测多选实体
			std::vector<Entity> multiSelected;
			auto view = m_Context->scene->GetAllofEntitiesWith<TransformComponent, ModelComponent>();
			for (auto [e, trans, model] : view.each()) {
				if (model.edgeEnable)
					multiSelected.emplace_back(e, m_Context->scene.get());
			}

			if (!multiSelected.empty())
				DrawMultiSelection(multiSelected);
			else
				ImGui::TextDisabled(u8"未选中任何实体");
		}

		ImGui::End();

		// 处理文件选择器的回调逻辑
		HandleFileDropCallbacks();
	}

	void InspectorPanel::DrawAllComponents(Entity entity)
	{
		DrawTagComponent(entity);
		DrawAddComponentButton(entity); // 添加组件按钮 & 弹窗

		DrawComponentWrapper<UUIDComponent>("UUID", entity, [](auto& component) {
			ImGui::TextDisabled("%s", ((std::string)component).c_str());
			});

		DrawTransformComponent(entity);
		DrawDirectionalLightComponent(entity);
		DrawPointLightComponent(entity);
		DrawModelComponent(entity);
	}

	void InspectorPanel::DrawMultiSelection(const std::vector<Entity>& entities)
	{
		ImGui::TextDisabled("多选模式: %zu 个实体", entities.size());
		ImGui::Separator();

		// --- 批量 Transform ---
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			glm::vec3 uiPos = m_Context->entitiesPosition;
			glm::vec3 uiRot = m_Context->entitiesRotation;
			glm::vec3 uiScale = m_Context->entitiesScale;

			// 记录旧值用于对比变化
			glm::vec3 oldPos = uiPos;
			glm::vec3 oldRot = uiRot;
			glm::vec3 oldScale = uiScale;

			DrawVec3Control("Position", uiPos);
			DrawVec3Control("Rotation", uiRot);
			DrawVec3Control("Scale", uiScale, glm::vec3(1.0f));

			// 应用变化
			if (uiPos != oldPos || uiRot != oldRot || uiScale != oldScale)
			{
				for (auto& entity : entities) {
					auto& tc = entity.GetComponent<TransformComponent>();
					if (uiPos != oldPos) {
						glm::vec3 delta = uiPos - oldPos;
						tc.position += delta;
						m_Context->entitiesPosition = uiPos;
					}
					if (uiRot != oldRot) {
						glm::vec3 delta = uiRot - oldRot;
						tc.rotation += delta;
						m_Context->entitiesRotation = uiRot;
					}
					if (uiScale != oldScale) {
						glm::vec3 delta = uiScale / oldScale;
						tc.scale *= delta;
						m_Context->entitiesScale = uiScale;
					}
				}
			}
		}

		// --- 批量 Model 可见性 ---
		if (ImGui::CollapsingHeader("Model (Batch)", ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool visible = entities[0].GetComponent<ModelComponent>().visible;
			if (ImGui::Checkbox("Visible", &visible)) {
				for (auto& entity : entities)
					entity.GetComponent<ModelComponent>().visible = visible;
			}
		}
	}

	// ---------------------------------------------------------
	// 组件的具体绘制逻辑
	// ---------------------------------------------------------

	void InspectorPanel::DrawTagComponent(Entity entity)
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
	}

	void InspectorPanel::DrawTransformComponent(Entity entity)
	{
		DrawComponentWrapper<TransformComponent>("Transform", entity, [this](auto& component) {
			DrawVec3Control("Position", component.position);
			DrawVec3Control("Rotation", component.rotation);
			DrawVec3Control("Scale", component.scale, glm::vec3(1.0f));
			});
	}

	void InspectorPanel::DrawDirectionalLightComponent(Entity entity)
	{
		DrawComponentWrapper<DirectionalLightComponent>("Directional Light", entity, [](auto& component) {
			if (ImGui::BeginTable("LightProps", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Control");

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Color");
				ImGui::TableSetColumnIndex(1); ImGui::ColorEdit4("##Color", glm::value_ptr(component.color));

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Direction");
				ImGui::TableSetColumnIndex(1);
				DrawVec3Control("Direction", component.direction, glm::vec3(0.0f, -1.0f, 0.0f));

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Ambient", &component.ambient, 0.01f, 0.0f, 1.0f);
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Diffuse");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Diffuse", &component.diffuse, 0.01f, 0.0f, 1.0f);
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Specular");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Specular", &component.specular, 0.01f, 0.0f, 1.0f);

				ImGui::EndTable();
			}
			});
	}

	void InspectorPanel::DrawPointLightComponent(Entity entity)
	{
		DrawComponentWrapper<PointLightComponent>("Point Light", entity, [](auto& component) {
			if (ImGui::BeginTable("LightProps", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp)) {
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Control");

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Color");
				ImGui::TableSetColumnIndex(1); ImGui::ColorEdit4("##Color", glm::value_ptr(component.color));

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Ambient");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Ambient", &component.ambient, 0.01f, 0.0f, 1.0f);
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Diffuse");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Diffuse", &component.diffuse, 0.01f, 0.0f, 1.0f);
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Specular");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Specular", &component.specular, 0.01f, 0.0f, 1.0f);

				ImGui::EndTable();
			}
			});
	}

	void InspectorPanel::DrawModelComponent(Entity entity)
	{
		if (!entity.HasAllofComponent<ModelComponent>()) return;

		DrawComponentWrapper<ModelComponent>("渲染网格", entity, [this](auto& component) {
			// 基础设置 (可见性、轮廓、Shader路径)
			if (ImGui::BeginTable("ModelSettings", 2, ImGuiTableFlags_BordersInnerV)) {
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Value");

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Visible");
				ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Visible", &component.visible);

				if (component.model->GetPrimitiveType() != PrimitiveType::Skybox) {
					ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Outline");
					ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Outline", &component.edgeEnable);
				}

				//ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::TextDisabled("Shader");
				//ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("%s", component.model->GetShaderPath().c_str());
				ImGui::EndTable();
			}

			ImGui::Separator();

			// 来源信息
			bool isImported = component.model->IsImported();
			if (isImported) {
				ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "Source: Imported");
				ImGui::TextWrapped("Path: %s", component.model->GetFullPath().c_str());
			}
			else {
				ImGui::TextColored({ 1.0f, 0.8f, 0.4f, 1.0f }, "Source: Primitive");
			}

			ImGui::Spacing();

			// 网格与纹理列表 (可折叠)
			if (ImGui::TreeNodeEx("Meshes & Textures", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
				auto& meshes = component.model->GetMeshes();
				for (size_t m = 0; m < meshes.size(); m++) {
					DrawMeshNode(m, meshes[m]);
				}
				ImGui::TreePop();
			}
			});
	}

	void InspectorPanel::DrawMeshNode(size_t meshIndex, const Refptr<Mesh>& mesh)
	{
		ImGuiTreeNodeFlags meshFlags = mesh->GetTextures().empty() ? 0 : ImGuiTreeNodeFlags_DefaultOpen;
		std::string meshLabel = "Mesh " + std::to_string(meshIndex) + " (" + PrimitiveTypeToString(mesh->GetPrimitiveType()) + ")";

		if (ImGui::TreeNodeEx(meshLabel.c_str(), meshFlags)) {

			// ------------------ 着色器 ---------------------
			std::string shaderPath = "Unknown Shader";
			if (auto& material = mesh->GetMaterial()) {
				if (auto shader = material->GetShader()) {
					shaderPath = shader->GetFilePath();
				}
			}

			ImGui::PushID(("MeshShader_" + std::to_string(meshIndex)).c_str());

			ImGui::TextDisabled("Shader"); // 标题

			// 计算布局：保留右侧 28像素 给按钮
			float availWidth = ImGui::GetContentRegionAvail().x;
			ImGui::PushItemWidth(availWidth - 28.0f);

			// 显示文件名
			std::filesystem::path p(shaderPath);
			std::string filename = p.filename().string();
			ImGui::TextDisabled("%s", filename.c_str());
			ImGui::PopItemWidth();

			ImGui::SameLine();

			// 替换按钮
			if (ImGui::Button("...", { 24.0f, 24.0f })) {
				m_Context->currentEditingMeshIndex = meshIndex;
				// 打开文件选择器，过滤 .glsl 文件
				FileSelecter::Open("EditMeshShader", "选择 Shader 文件", "(.glsl){.glsl},.glsl");
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("替换此 Mesh 的 Shader");

			ImGui::PopID();
			ImGui::Spacing(); // 增加一点间距

			// --------------- 纹理列表 ---------------------
			DrawTextureTable(meshIndex, mesh->GetTextures(), const_cast<Refptr<Mesh>&>(mesh));

			ImGui::Spacing();

			// 添加新纹理按钮
			if (ImGui::Button(" + 添加新纹理 ", { -1, 28 })) {
				ImGui::OpenPopup("SelectTextureUsagePopup");
			}

			if (ImGui::BeginPopup("SelectTextureUsagePopup"))
			{
				ImGui::TextDisabled("选择纹理用途");
				ImGui::Separator();

				auto AddTextureMenuItem = [&](const char* label, TextureUsage usage) {
					if (ImGui::MenuItem(label)) {
						m_Context->currentEditingMeshIndex = meshIndex;
						m_Context->pendingTextureUsage = usage;
						if (usage == TextureUsage::Cubemap) {
							std::vector<std::string> defaultPaths = {
								"assets/images/defaultSky/right.jpg",
								"assets/images/defaultSky/left.jpg",
								"assets/images/defaultSky/top.jpg",
								"assets/images/defaultSky/bottom.jpg",
								"assets/images/defaultSky/front.jpg",
								"assets/images/defaultSky/back.jpg"
							};
							if (auto newTexture = TextureLibrary::Load("NewCubemap", defaultPaths, TextureUsage::Cubemap)) {
								// 获取当前 Mesh 并添加纹理
								auto& modelComp = m_Context->selectedEntity.GetComponent<ModelComponent>();
								auto& meshes = modelComp.model->GetMeshes();

								if (meshIndex < meshes.size()) {
									meshes[meshIndex]->AddTexture(newTexture, usage);
								}
							}
						}
						else {
							FileSelecter::Open("LoadNewTexture", "导入新纹理", "(.png,.jpg,.jpeg,.tga,.bmp){.png,.jpg,.jpeg,.tga,.bmp},.png,.jpg,.jpeg,.tga,.bmp");
						}
						ImGui::CloseCurrentPopup();
					}
					};

				AddTextureMenuItem("Diffuse / Albedo", TextureUsage::Diffuse);
				AddTextureMenuItem("Specular / Metallic", TextureUsage::Specular);
				AddTextureMenuItem("Normal", TextureUsage::Normal);
				AddTextureMenuItem("Cube Map", TextureUsage::Cubemap);
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}

	void InspectorPanel::DrawTextureTable(size_t meshIndex, const std::vector<Refptr<Texture>>& textures, Refptr<Mesh>& mesh)
	{
		if (textures.empty()) return;

		int textureIndexToRemove = -1;

		if (ImGui::BeginTable("TextureSlots", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
			ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, 64.0f);
			ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Edit", ImGuiTableColumnFlags_WidthFixed, 40.0f);

			for (size_t t = 0; t < textures.size(); t++) {
				auto& texture = textures[t];
				ImGui::PushID(static_cast<int>(t));

				ImGui::TableNextRow();

				if (texture->GetPath().size() == 1) {
					// Column 0: 预览图 (可点击替换)
					ImGui::TableSetColumnIndex(0);
					uint32_t textureId = texture->GetRendererId();
					if (ImGui::ImageButton("##texBtn", (void*)(intptr_t)textureId, { 56.0f, 56.0f }, { 0, 1 }, { 1, 0 })) {
						m_Context->currentEditingMeshIndex = meshIndex;
						m_Context->currentEditingTexIndex = t;
						FileSelecter::Open("EditTexture2D", "选择纹理", "(.png,.jpg,.jpeg,.tga,.bmp){.png,.jpg,.jpeg,.tga,.bmp},.png,.jpg,.jpeg,.tga,.bmp");
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("点击更换纹理");

					// Column 1: 信息文本
					ImGui::TableSetColumnIndex(1);
					ImGui::TextColored({ 0.8f, 0.8f, 0.8f, 1.0f }, "%s", TextureUsageToString(texture->GetUsage()).c_str());
					ImGui::TextDisabled("%s", TextureTypeToString(texture->GetType()).c_str());
					if (!texture->GetPath().empty())
						ImGui::TextWrapped("%s", texture->GetPath()[0].c_str());

					// Column 2: 删除按钮
					ImGui::TableSetColumnIndex(2);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
					if (ImGui::Button("X", { 24, 24 })) {
						textureIndexToRemove = static_cast<int>(t);
					}
					ImGui::PopStyleColor(2);
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("移除此纹理");
				}
				else if (texture->GetPath().size() > 1) {
					// Cubemap 处理逻辑

					ImGui::TableSetColumnIndex(0);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.4f, 1.0f));
					ImGui::Button("Cube", { 56.0f, 56.0f }); // 占位符，或者简单的 Cube 图标
					ImGui::PopStyleColor();
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Cubemap 包含 6 个面");

					// Column 1: 信息文本 + 展开 6 面
					ImGui::TableSetColumnIndex(1);
					ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "%s", TextureUsageToString(texture->GetUsage()).c_str());

					// 使用 TreeNode 显示 6 个面的路径
					bool open = ImGui::TreeNodeEx("##cubemap_faces", ImGuiTreeNodeFlags_SpanAvailWidth, "包含 6 个纹理面");

					if (open) {
						const std::vector<std::string>& paths = texture->GetPath();
						// 定义面的名称，顺序通常是: Right, Left, Top, Bottom, Front, Back
						const char* faceNames[] = { "右侧", "左侧", "顶部", "底部", "前方", "后方" };

						// 确保路径数量正确，防止越界
						size_t count = std::min(paths.size(), (size_t)6);

						for (size_t i = 0; i < count; i++) {
							ImGui::PushID(static_cast<int>(i));

							// 面名称
							ImGui::AlignTextToFramePadding();
							ImGui::TextDisabled("%-12s", faceNames[i]);
							ImGui::SameLine();

							// 该面的2D预览图
							auto previewTex = TextureLibrary::Load("TempSkyboxPreview_" + paths[i], { paths[i] }, TextureUsage::None);
							uint32_t texId = previewTex ? previewTex->GetRendererId() : 0;

							// 图像按钮
							if (ImGui::ImageButton("##FaceBtn", (void*)(intptr_t)texId, { 64.0f, 64.0f }, { 0, 1 }, { 1, 0 })) {
								m_Context->currentEditingMeshIndex = meshIndex;
								m_Context->currentEditingTexIndex = t;
								m_Context->currentEditingFaceIndex = static_cast<int>(i);
								FileSelecter::Open("EditCubemapFace", "替换贴图面", "(.png,.jpg,.jpeg,.tga,.bmp){.png,.jpg,.jpeg,.tga,.bmp},.png,.jpg,.jpeg,.tga,.bmp");
							}

							// 悬浮提示路径
							if (ImGui::IsItemHovered()) {
								ImGui::SetTooltip("%s", paths[i].c_str());
							}

							ImGui::PopID();
						}
						ImGui::TreePop();
					}

					// Column 2: 删除按钮 (整个 Cubemap 一起删)
					ImGui::TableSetColumnIndex(2);
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.4f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
					if (ImGui::Button("X", { 24, 24 })) {
						textureIndexToRemove = static_cast<int>(t);
					}
					ImGui::PopStyleColor(2);
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("移除此 Cubemap");
				}

				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		if (textureIndexToRemove != -1) {
			mesh->RemoveTexture(textureIndexToRemove);
		}
	}

	void InspectorPanel::HandleFileDropCallbacks()
	{
		// 1. 导入新模型到实体
		FileSelecter::Handle("ModelImportKey", [this](const std::string& path) {
			if (m_Context->selectedEntity) {
				auto shader = ShaderLibrary::Load("assets/shaders/Standard.glsl", {});
				auto model = std::make_shared<Model>(shader, path);
				m_Context->selectedEntity.AddComponent<ModelComponent>(model);
			}
			});

		// 2. 替换现有纹理
		FileSelecter::Handle("EditTexture2D", [this](const std::string& path) {
			auto entity = m_Context->selectedEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();
			size_t mIdx = m_Context->currentEditingMeshIndex;
			size_t tIdx = m_Context->currentEditingTexIndex;

			if (mIdx < meshes.size()) {
				meshes[mIdx]->EditTexture(tIdx, { path });
			}
			});

		// 3. 加载新纹理
		FileSelecter::Handle("LoadNewTexture", [this](const std::string& path) {
			auto entity = m_Context->selectedEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();
			size_t mIdx = m_Context->currentEditingMeshIndex;

			if (mIdx < meshes.size()) {
				TextureUsage targetUsage = m_Context->pendingTextureUsage;
				if (auto newTexture = TextureLibrary::Load({ path }, targetUsage)) {
					meshes[mIdx]->AddTexture(newTexture, targetUsage);
				}
			}
			});

		// 4. 替换 Cubemap 的某一面
		FileSelecter::Handle("EditCubemapFace", [this](const std::string& path) {
			auto entity = m_Context->selectedEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();

			size_t mIdx = m_Context->currentEditingMeshIndex;
			size_t tIdx = m_Context->currentEditingTexIndex;
			int faceIdx = m_Context->currentEditingFaceIndex; // 获取记录的面索引

			if (mIdx < meshes.size()) {
				auto& mesh = meshes[mIdx];
				auto textures = mesh->GetTextures(); // 获取当前纹理列表 (副本)

				if (tIdx < textures.size()) {
					auto& oldTexture = textures[tIdx];
					// Cubemap (路径 > 1)
					if (oldTexture->GetPath().size() > 1) {
						std::vector<std::string> newPaths = oldTexture->GetPath();

						if (faceIdx >= 0 && faceIdx < newPaths.size()) {
							newPaths[faceIdx] = path;

							mesh->EditTexture(tIdx, newPaths);
						}
					}
				}
			}
			});

		// 5. 替换 Mesh 的 Shader
		FileSelecter::Handle("EditMeshShader", [this](const std::string& path) {
			auto entity = m_Context->selectedEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();
			size_t mIdx = m_Context->currentEditingMeshIndex;

			if (mIdx < meshes.size()) {
				// 调用 Mesh 的接口替换 Shader
				meshes[mIdx]->EditShader(path);
			}
			});
	}

	void InspectorPanel::DrawAddComponentButton(Entity entity)
	{
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::Spacing();

		if (ImGui::Button("添加组件...", { -1, 24 }))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			// --------------- 添加模型组件 -------------------
			if (ImGui::BeginMenu("模型组件"))
			{
				if (ImGui::BeginMenu("基础图元"))
				{
					auto AddPrimitive = [&](PrimitiveType type, const std::string& name) {
						if (m_Context->selectedEntity.HasAllofComponent<ModelComponent>()) return;

						if (type != PrimitiveType::Skybox) {
							auto defaultShader = ShaderLibrary::Load("assets/shaders/Standard.glsl", {});
							auto model = std::make_shared<Model>(type, defaultShader, std::vector<Refptr<Texture>>{});
							m_Context->selectedEntity.AddComponent<ModelComponent>(model);
						}
						else {
							// Skybox 特殊处理
							auto skyShader = ShaderLibrary::Load("assets/shaders/TextureCube_Shader.glsl", {});
							std::vector<std::string> paths = {
								"assets/images/defaultSky/right.jpg", "assets/images/defaultSky/left.jpg",
								"assets/images/defaultSky/top.jpg",   "assets/images/defaultSky/bottom.jpg",
								"assets/images/defaultSky/front.jpg", "assets/images/defaultSky/back.jpg"
							};
							if (auto texture = TextureLibrary::Load("DefaultSkyboxTextures", paths, TextureUsage::Cubemap)) {
								std::vector<Refptr<Texture>> texList = { texture };
								auto model = std::make_shared<Model>(type, skyShader, texList);
								m_Context->selectedEntity.AddComponent<ModelComponent>(model);
								m_Context->selectedEntity.RemoveComponent<TransformComponent>();
							}
						}
						ImGui::CloseCurrentPopup();
						};

					if (ImGui::MenuItem("Cube"))   AddPrimitive(PrimitiveType::Cube, "Cube");
					if (ImGui::MenuItem("Sphere")) AddPrimitive(PrimitiveType::Sphere, "Sphere");
					if (ImGui::MenuItem("Plane"))  AddPrimitive(PrimitiveType::Plane, "Plane");
					if (ImGui::MenuItem("Skybox")) AddPrimitive(PrimitiveType::Skybox, "Skybox");
					ImGui::EndMenu();
				}
								
				if (ImGui::MenuItem("导入外部模型..."))
				{
					FileSelecter::Open("ModelImportKey", "导入模型", "(.obj,.fbx,.gltf){.obj,.fbx,.gltf},.obj,.fbx,.gltf");
				}
				ImGui::EndMenu();
			}

			// ------------ 添加光源组件 -----------
			if (ImGui::BeginMenu("光源组件"))
			{
				// ### 平行光源组件
				if (ImGui::BeginMenu("平行光源组件"))
				{
					static glm::vec4 s_InitColor = { 1.0f, 1.0f, 1.0f, 1.0f };
					static glm::vec3 s_InitDir = { 0.0f, -1.0f, 0.0f }; // 竖直向下
					static float s_InitAmbient = 0.1f;
					static float s_InitDiffuse = 0.8f;
					static float s_InitSpecular = 0.5f;
					ImGui::TextDisabled("初始参数设置");

					ImGui::ColorEdit4("颜色", glm::value_ptr(s_InitColor));
					DrawVec3Control("方向", s_InitDir, glm::vec3(0.0f, -1.0f, 0.0f));
					ImGui::TextDisabled("光强参数");
					ImGui::DragFloat("环境光强度", &s_InitAmbient, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("漫反射光强度", &s_InitDiffuse, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("镜面光强度", &s_InitSpecular, 0.01f, 0.0f, 1.0f);

					ImGui::Separator();

					// 实际执行添加的按钮
					if (ImGui::MenuItem("确认添加"))
					{
						if (m_Context->selectedEntity && !m_Context->selectedEntity.HasAllofComponent<DirectionalLightComponent>())
						{
							auto& dlc = m_Context->selectedEntity.AddComponent<DirectionalLightComponent>();

							// 删除transform组件和model组件
							if(m_Context->selectedEntity.HasAllofComponent<TransformComponent>()) 
								m_Context->selectedEntity.RemoveComponent<TransformComponent>();
							if (m_Context->selectedEntity.HasAllofComponent<ModelComponent>())
								m_Context->selectedEntity.RemoveComponent<ModelComponent>();

							dlc.color = s_InitColor;
							dlc.direction = s_InitDir;

							dlc.ambient = s_InitAmbient;
							dlc.diffuse = s_InitDiffuse;
							dlc.specular = s_InitSpecular;

							s_InitColor = { 1.0f, 1.0f, 1.0f, 1.0f };
							s_InitDir = { 0.0f, -1.0f, 0.0f };
							static float s_InitAmbient = 0.1f;
							static float s_InitDiffuse = 0.8f;
							static float s_InitSpecular = 0.5f;
						}
						ImGui::CloseCurrentPopup(); // 关闭整个 "AddComponent" 菜单
					}

					ImGui::EndMenu();
				}

				// ### 点光源组件 
				if (ImGui::BeginMenu("点光源组件"))
				{
					static glm::vec4 s_InitColor = { 1.0f, 1.0f, 1.0f, 1.0f };
					static float s_InitAmbient = 0.1f;
					static float s_InitDiffuse = 0.8f;
					static float s_InitSpecular = 0.5f;
					ImGui::TextDisabled("初始参数设置");
					// 颜色选择面板
					ImGui::ColorEdit4("颜色", glm::value_ptr(s_InitColor));
					ImGui::TextDisabled("光强参数");
					ImGui::DragFloat("环境光强度", &s_InitAmbient, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("漫反射光强度", &s_InitDiffuse, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("镜面光强度", &s_InitSpecular, 0.01f, 0.0f, 1.0f);

					ImGui::Separator();

					// 实际执行添加的按钮
					if (ImGui::MenuItem("确认添加"))
					{
						if (m_Context->selectedEntity && !m_Context->selectedEntity.HasAllofComponent<PointLightComponent>())
						{
							auto& plc = m_Context->selectedEntity.AddComponent<PointLightComponent>();
							plc.color = s_InitColor;
							
							plc.ambient = s_InitAmbient;
							plc.diffuse = s_InitDiffuse;
							plc.specular = s_InitSpecular;

							s_InitColor = { 1.0f, 1.0f, 1.0f, 1.0f };
							static float s_InitAmbient = 0.1f;
							static float s_InitDiffuse = 0.8f;
							static float s_InitSpecular = 0.5f;
						}
						ImGui::CloseCurrentPopup(); // 关闭整个 "AddComponent" 菜单
					}

					ImGui::EndMenu();
				}

				
				ImGui::EndMenu();
			}



			ImGui::EndPopup();
		}
		ImGui::PopItemWidth();
	}

	void InspectorPanel::DrawVec3Control(const std::string& label, glm::vec3& values, glm::vec3& resetValue, float columnWidth)
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

		// X Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		if (ImGui::Button("X", buttonSize)) values.x = resetValue.x;
		ImGui::PopStyleColor(2);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue.y;
		ImGui::PopStyleColor(2);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue.z;
		ImGui::PopStyleColor(2);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	void InspectorPanel::DrawComponentWrapper(const std::string& name, Entity entity, UIFunction uiFunction)
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