#include "InspectorPanel.h"

#include "Snail/Utils/FileSelecter.h"

#include <glm/gtc/type_ptr.hpp>

namespace Snail {

	InspectorPanel::InspectorPanel(const Refptr<EditorContext>& context)
		: m_Context(context)
	{
		// --- 设置回调函数 ---
		// 替换现有纹理
		SetEditTexture2DCallback([this](const std::string& path) {
			auto entity = m_Context->displayEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();
			size_t mIdx = m_Context->currentEditingMeshIndex;
			size_t tIdx = m_Context->currentEditingTexIndex;

			if (mIdx < meshes.size()) {
				meshes[mIdx]->EditTexture(tIdx, { path });
			}
			});

		// 加载新纹理
		SetCreateTextureCallback([this](const std::string& path) {
			auto entity = m_Context->displayEntity;
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

		// 替换整个 cube map
		SetEditTextureCubeCallback_Entirely([this](const std::string& path) {
			auto entity = m_Context->displayEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();

			size_t mIdx = m_Context->currentEditingMeshIndex;
			size_t tIdx = m_Context->currentEditingTexIndex;

			if (mIdx < meshes.size()) {
				meshes[mIdx]->EditTexture(tIdx, { path });
			}
			});

		// 替换 Cubemap 的某一面
		SetEditTextureCubeCallback([this](const std::string& path) {
			auto entity = m_Context->displayEntity;
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

		// 替换 Mesh 的 Shader
		SetMeshShaderFileOpenCallback([this](const std::string& path) {
			auto entity = m_Context->displayEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();
			size_t mIdx = m_Context->currentEditingMeshIndex;

			if (mIdx < meshes.size()) {
				// 调用 Mesh 的接口替换 Shader
				meshes[mIdx]->EditShader(path);
			}
			});

		// 替换 Model 的 Shader
		SetModelShaderFileOpenCallback([this](const std::string& path) {
			auto entity = m_Context->displayEntity;
			if (!entity || !entity.HasAllofComponent<ModelComponent>()) return;

			auto& modelComp = entity.GetComponent<ModelComponent>();
			auto& meshes = modelComp.model->GetMeshes();

			if (meshes.size())
				for (auto& mesh : meshes) mesh->EditShader(path);

			modelComp.model->SetDefaultShaderPath(path);
			// ----------------- 调试代码 -----------------
			SNL_CORE_WARN("Modify Model Addr: {0}, DefaultShaderPath: {1}",
				(void*)modelComp.model.get(),
				modelComp.model->GetDefaultShaderPath());
			// -------------------------------------------
			});
	}

	void InspectorPanel::Show()
	{
		ImGui::Begin(u8"属性面板");

		if (m_Context->displayEntity && m_Context->displayEntity.IsValid())
		{
			DrawAllComponents(m_Context->displayEntity);
		}
		else
		{
			// 多选实体
			std::vector<Entity> multiSelected;
			for (auto& e : m_Context->selectedEntities) {
				if(e.HasAllofComponent<TransformComponent, ModelComponent>())
					multiSelected.push_back(e);
			}

			if (multiSelected.size() > 1)
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
		if (ImGui::CollapsingHeader("Model (多个)", ImGuiTreeNodeFlags_DefaultOpen))
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
				ImGui::TableSetColumnIndex(1); ImGui::ColorEdit3("##Color", glm::value_ptr(component.color));

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Direction");
				ImGui::TableSetColumnIndex(1);
				DrawVec3Control("Direction", component.direction, glm::vec3(0.0f, -1.0f, 0.0f));

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Intensity");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Intensity", &component.intensity, 0.01f, 0.0f, 10.0f);

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
				ImGui::TableSetColumnIndex(1); ImGui::ColorEdit3("##Color", glm::value_ptr(component.color));

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Intensity");
				ImGui::TableSetColumnIndex(1); ImGui::DragFloat("##Intensity", &component.intensity, 1.0f, 0.0f, 10000.0f);

				ImGui::EndTable();
			}
			});
	}

	void InspectorPanel::DrawModelComponent(Entity entity)
	{
		if (!entity.HasAllofComponent<ModelComponent>()) return;

		DrawComponentWrapper<ModelComponent>("Model", entity, [this](auto& component) {
			// 基础设置 (可见性、轮廓、Shader路径)
			if (ImGui::BeginTable("ModelSettings", 2, ImGuiTableFlags_BordersInnerV)) {
				ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 150.0f);
				ImGui::TableSetupColumn("Value");

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Visible");
				ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Visible", &component.visible);

				if (component.model->GetPrimitiveType() != PrimitiveType::Skybox) {
					ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("Outline");
					ImGui::TableSetColumnIndex(1); ImGui::Checkbox("##Outline", &component.edgeEnable);
				}

				// 显示文件名
				std::filesystem::path p(component.model->GetDefaultShaderPath());
				std::string filename = p.filename().string();
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::TextDisabled("Model Shader");
				ImGui::TableSetColumnIndex(1); ImGui::TextDisabled("%s", filename.c_str());
				// ---------------- 总shader路径作为拖拽目标 -------------------
				DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
					std::string extension = path.extension().string();
					if (extension == ".glsl")
					{
						// 编辑（替换）Model着色器的callback
						m_OnModelShaderFileOpenCallback(path.string());
					}
					});

				ImGui::SameLine();

				// 替换按钮
				if (ImGui::Button("...", { 24.0f, 24.0f })) {
					// 打开文件选择器，过滤 .glsl 文件
					FileSelecter::Open("EditModelShader", "选择 Shader 文件", "(.glsl){.glsl},.glsl");
				}
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("替换此 Model 的 Shader");

				// --- 纹理控制 ---
				bool enableTexture = component.model->GetEnableTextures();
				if (ImGui::Checkbox(u8"启用 Model 全部纹理", &enableTexture)) {
					component.model->SetEnableTextures(enableTexture);
					for (auto& mesh : component.model->GetMeshes())
					{
						mesh->SetEnableTextures(enableTexture);
						for (auto& texture : mesh->GetTextures())
							texture->SetEnable(enableTexture);
						mesh->RemapMaterialTextures();
					}
				}

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

			// --- 材质参数 ---
			if (ImGui::TreeNodeEx(u8"基础材质参数", ImGuiTreeNodeFlags_Framed)) {
				auto firstMaterial = component.model->GetMeshes()[0]->GetMaterial();
				ImGui::TextDisabled("———— Phong 管线 ————");
				// Ambient
				glm::vec3 ambient = firstMaterial->GetAmbientColor();
				if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient))) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetAmbientColor(ambient);
					}
				}
				// Diffuse
				glm::vec3 diffuse = firstMaterial->GetDiffuseColor();
				if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse))) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetDiffuseColor(diffuse);
					}
				}
				// Specular
				glm::vec3 specular = firstMaterial->GetSpecularColor();
				if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular))) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetSpecularColor(specular);
					}
				}
				// Shininess
				float shininess = firstMaterial->GetShininess();
				if (ImGui::SliderFloat("Shininess", &shininess, 0.0f, 1000.0f)) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetShininess(shininess);
					}
				}

				ImGui::TextDisabled("———— PBR 管线 ————");
				// Albedo
				glm::vec3 albedo = firstMaterial->GetAlbedoColor();
				if (ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo))) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetAlbedoColor(albedo);
					}
				}
				// Roughness
				float rough = firstMaterial->GetRoughness();
				if (ImGui::SliderFloat("Roughness", &rough, 0.0f, 1.0f)) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetRoughness(rough);
					}
				}
				// Metallic
				float metal = firstMaterial->GetMetallic();
				if (ImGui::SliderFloat("Metallic", &metal, 0.0f, 1.0f)) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetMetallic(metal);
					}
				}
				// AO
				float ao = firstMaterial->GetAO();
				if (ImGui::SliderFloat("AO", &ao, 0.0f, 1.0f)) {
					for (auto& mesh : component.model->GetMeshes()) {
						mesh->GetMaterial()->SetAO(ao);
					}
				}
				ImGui::TreePop();
			}

			ImGui::Spacing();

			// 网格与纹理列表 (可折叠)
			if (ImGui::TreeNodeEx(u8"网格 & 纹理", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed)) {
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

			ImGui::TextDisabled("Mesh Shader"); // 标题

			// 计算布局：保留右侧 28像素 给按钮
			float availWidth = ImGui::GetContentRegionAvail().x;
			ImGui::PushItemWidth(availWidth - 28.0f);

			// 显示文件名
			std::filesystem::path p(shaderPath);
			std::string filename = p.filename().string();
			ImGui::TextDisabled("%s", filename.c_str());
			ImGui::PopItemWidth();

			// ---------------- shader路径作为拖拽目标 -------------------
			DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
				std::string extension = path.extension().string();
				if (extension == ".glsl")
				{
					m_Context->currentEditingMeshIndex = meshIndex;
					// 编辑（替换）Mesh着色器的callback
					m_OnMeshShaderFileOpenCallback(path.string());
				}
				});

			ImGui::SameLine();

			// 替换按钮
			if (ImGui::Button("...", { 24.0f, 24.0f })) {
				m_Context->currentEditingMeshIndex = meshIndex;
				// 打开文件选择器，过滤 .glsl 文件
				FileSelecter::Open("EditMeshShader", "选择 Shader 文件", "(.glsl){.glsl},.glsl");
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("替换此 Mesh 的 Shader");

			// --- 纹理控制 ---
			bool enableTexture = mesh->GetEnableTextures();
			if (ImGui::Checkbox(u8"启用 Mesh 全部纹理", &enableTexture)) {
				mesh->SetEnableTextures(enableTexture);
				for (auto& texture : mesh->GetTextures())
					texture->SetEnable(enableTexture);
				mesh->RemapMaterialTextures();
			}

			// --- 材质参数 ---
			if (ImGui::TreeNodeEx(u8"Mesh 基础材质参数", ImGuiTreeNodeFlags_Framed)) {
				auto firstMaterial = mesh->GetMaterial();
				ImGui::TextDisabled("———— Phong 管线 ————");
				// Ambient
				glm::vec3 ambient = firstMaterial->GetAmbientColor();
				if (ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient))) {
					mesh->GetMaterial()->SetAmbientColor(ambient);
				}
				// Diffuse
				glm::vec3 diffuse = firstMaterial->GetDiffuseColor();
				if (ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse))) {
					mesh->GetMaterial()->SetDiffuseColor(diffuse);
				}
				// Specular
				glm::vec3 specular = firstMaterial->GetSpecularColor();
				if (ImGui::ColorEdit3("Specular", glm::value_ptr(specular))) {
					mesh->GetMaterial()->SetSpecularColor(specular);
				}
				// Shininess
				float shininess = firstMaterial->GetShininess();
				if (ImGui::SliderFloat("Shininess", &shininess, 0.0f, 1000.0f)) {
					mesh->GetMaterial()->SetShininess(shininess);
				}

				ImGui::TextDisabled("———— PBR 管线 ————");
				// Albedo
				glm::vec3 albedo = firstMaterial->GetAlbedoColor();
				if (ImGui::ColorEdit3("Albedo", glm::value_ptr(albedo))) {
					mesh->GetMaterial()->SetAlbedoColor(albedo);
				}
				// Roughness
				float rough = firstMaterial->GetRoughness();
				if (ImGui::SliderFloat("Roughness", &rough, 0.0f, 1.0f)) {
					mesh->GetMaterial()->SetRoughness(rough);
				}
				// Metallic
				float metal = firstMaterial->GetMetallic();
				if (ImGui::SliderFloat("Metallic", &metal, 0.0f, 1.0f)) {
					mesh->GetMaterial()->SetMetallic(metal);
				}
				// AO
				float ao = firstMaterial->GetAO();
				if (ImGui::SliderFloat("AO", &ao, 0.0f, 1.0f)) {
					mesh->GetMaterial()->SetAO(ao);
				}
				ImGui::TreePop();
			}

			ImGui::PopID();
			ImGui::Spacing(); // 增加一点间距

			// --------------- 纹理列表 ---------------------
			DrawTextureTable(meshIndex, mesh->GetTextures(), const_cast<Refptr<Mesh>&>(mesh));

			ImGui::Spacing();

			// 添加新纹理按钮
			if (ImGui::Button(" + 添加新纹理 ", { -1, 28 })) {
				ImGui::OpenPopup("SelectTextureUsagePopup");
			}

			// ---------------- 添加纹理的 按钮 作为拖拽目标 -------------------
			DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
				std::string extension = path.extension().string();
				if (extension == ".png" || extension == ".jpg" || extension == ".bmp" || extension == ".tga" || extension == ".hdr" || extension == ".exr")
				{
					m_Context->currentEditingMeshIndex = meshIndex;
					m_Context->pendingTextureUsage = TextureUsage::Diffuse; // TODO: 暂时设置成默认diffuse
					// 新增纹理的callback
					m_OnCreateTextureCallback(path.string());
				}
				});

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
								auto& modelComp = m_Context->displayEntity.GetComponent<ModelComponent>();
								auto& meshes = modelComp.model->GetMeshes();

								if (meshIndex < meshes.size()) {
									meshes[meshIndex]->AddTexture(newTexture, usage);
								}
							}
						}
						else {
							//FileSelecter::Open("LoadNewTexture", "导入新纹理", "(.png,.jpg,.bmp,.tga,.hdr,.exr){.png,.jpg,.bmp,.tga,.hdr,.exr},.png,.jpg,.bmp,.tga,.hdr,.exr");
							m_OnCreateTextureCallback("assets/images/default.png");
						}
						ImGui::CloseCurrentPopup();
					}
					};

				AddTextureMenuItem("Diffuse", TextureUsage::Diffuse);
				AddTextureMenuItem("Specular", TextureUsage::Specular);
				AddTextureMenuItem("Cube Map", TextureUsage::Cubemap);

				AddTextureMenuItem("Normal", TextureUsage::Normal);
				AddTextureMenuItem("Albedo", TextureUsage::Albedo);
				AddTextureMenuItem("Metallic", TextureUsage::Metallic);
				AddTextureMenuItem("Roughness", TextureUsage::Roughness);
				AddTextureMenuItem("AO", TextureUsage::AO);
				ImGui::EndPopup();
			}
			ImGui::TreePop();
		}
	}

	void InspectorPanel::DrawTextureTable(size_t meshIndex, const std::vector<Refptr<Texture>>& textures, Refptr<Mesh>& mesh)
	{
		if (textures.empty()) return;

		int textureIndexToRemove = -1;

		if (ImGui::BeginTable("TextureSlots", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
			ImGui::TableSetupColumn("Content", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 40.0f);

			for (size_t t = 0; t < textures.size(); t++) {
				auto& texture = textures[t];
				ImGui::PushID(static_cast<int>(t));

				ImGui::TableNextRow();

				// --- Column 0: Content ---
				ImGui::TableSetColumnIndex(0);

				if (texture->GetPath().size() == 1 && texture->GetUsage() != TextureUsage::Cubemap) {
					// 2D Texture: 预览图 + 右侧文字
					uint32_t textureId = texture->GetUIRendererId();
					if (ImGui::ImageButton("##texBtn", (void*)(intptr_t)textureId, { 56.0f, 56.0f }, { 0, 1 }, { 1, 0 })) {
						m_Context->currentEditingMeshIndex = meshIndex;
						m_Context->currentEditingTexIndex = t;
						FileSelecter::Open("EditTexture2D", "选择纹理", "(.png,.jpg,.bmp,.tga,.hdr,.exr){.png,.jpg,.bmp,.tga,.hdr,.exr},.png,.jpg,.bmp,.tga,.hdr,.exr");
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("点击更换纹理");

					// --- 普通2D纹理 预览图 作为拖拽对象
					DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
						std::string extension = path.extension().string();
						if (extension == ".png" || extension == ".jpg" || extension == ".bmp" || extension == ".tga" || extension == ".hdr" || extension == ".exr") {
							m_Context->currentEditingMeshIndex = meshIndex;
							m_Context->currentEditingTexIndex = t;
							m_OnEditTexture2DCallback(path.string());
						}
						});

					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::TextColored({ 1.0f, 0.8f, 0.4f, 1.0f }, "%s", TextureUsageToString(texture->GetUsage()).c_str());
					ImGui::TextDisabled("Type: %s", TextureTypeToString(texture->GetType()).c_str());
					if (!texture->GetPath().empty())
						ImGui::TextWrapped("%s", std::filesystem::path(texture->GetPath()[0]).filename().string().c_str());
					ImGui::EndGroup();
				}
				else if (texture->GetPath().size() == 1 && texture->GetUsage() == TextureUsage::Cubemap) {
					// Equirectangular Cubemap: 大图 + 下方文字
					auto previewTex = TextureLibrary::Load({ texture->GetPath() }, TextureUsage::UI);
					uint32_t texId = previewTex ? previewTex->GetRendererId() : 0;

					if (ImGui::ImageButton("##CubemapBtn", (void*)(intptr_t)texId, { 256.0f, 128.0f }, { 0, 1 }, { 1, 0 })) {
						m_Context->currentEditingMeshIndex = meshIndex;
						m_Context->currentEditingTexIndex = t;
						FileSelecter::Open("EditTextureCube", "替换立方体贴图", "(.hdr,.exr){.hdr,.exr},.hdr,.exr");
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("点击更换立方体贴图");

					// --- 整个立方体贴图作为拖拽对象 ---
					DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
						std::string extension = path.extension().string();
						if (extension == ".hdr" || extension == ".exr") {
							m_Context->currentEditingMeshIndex = meshIndex;
							m_Context->currentEditingTexIndex = t;
							m_OnEditTextureCubeCallback_Entirely(path.string());
						}
						});

					ImGui::Spacing();
					ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "%s", TextureUsageToString(texture->GetUsage()).c_str());
					ImGui::TextDisabled("Type: %s", TextureTypeToString(texture->GetType()).c_str());
					if (!texture->GetPath().empty())
						ImGui::TextWrapped("路径: %s", texture->GetPath()[0].c_str());
				}
				else if (texture->GetPath().size() > 1) {
					// 6-Sided Cubemap
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.4f, 1.0f));
					if (ImGui::Button("Cube", { 56.0f, 56.0f })) {
						m_Context->currentEditingMeshIndex = meshIndex;
						m_Context->currentEditingTexIndex = t;
						FileSelecter::Open("EditTextureCube", "替换立方体贴图", "(.hdr,.exr){.hdr,.exr},.hdr,.exr");
					}
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("点击更换立方体贴图");
					ImGui::PopStyleColor();

					// --- 整个立方体贴图作为拖拽对象 ---
					DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
						std::string extension = path.extension().string();
						if (extension == ".hdr" || extension == ".exr") {
							m_Context->currentEditingMeshIndex = meshIndex;
							m_Context->currentEditingTexIndex = t;
							m_OnEditTextureCubeCallback_Entirely(path.string());
						}
						});

					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::TextColored({ 0.4f, 0.8f, 1.0f, 1.0f }, "%s", TextureUsageToString(texture->GetUsage()).c_str());

					bool open = ImGui::TreeNodeEx("##cubemap_faces", ImGuiTreeNodeFlags_SpanAvailWidth, "贴图面数(6)");
					if (open) {
						const auto& paths = texture->GetPath();
						const char* faceNames[] = { "右侧", "左侧", "顶部", "底部", "前方", "后方" };
						size_t count = std::min(paths.size(), (size_t)6);

						for (size_t i = 0; i < count; i++) {
							ImGui::PushID((int)i);
							ImGui::AlignTextToFramePadding();
							ImGui::TextDisabled("%-8s", faceNames[i]);
							ImGui::SameLine();

							auto facePreview = TextureLibrary::Load("Temp_Face_Prev_" + paths[i], { paths[i] }, TextureUsage::UI);
							uint32_t fId = facePreview ? facePreview->GetRendererId() : 0;

							if (ImGui::ImageButton("##FaceBtn", (void*)(intptr_t)fId, { 56.0f, 56.0f }, { 0, 1 }, { 1, 0 })) {
								m_Context->currentEditingMeshIndex = meshIndex;
								m_Context->currentEditingTexIndex = t;
								m_Context->currentEditingFaceIndex = (int)i;
								FileSelecter::Open("EditCubemapFace", "替换贴图单面", "(.png,.jpg,.bmp,.tga,.hdr,.exr){.png,.jpg,.bmp,.tga,.hdr,.exr},.png,.jpg,.bmp,.tga,.hdr,.exr");
							}
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("点击更换立方体贴图单面");

							// --- 立方体贴图 单面 作为拖拽对象 ---
							DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
								std::string extension = path.extension().string();
								if (extension == ".hdr" || extension == ".exr") {
									m_Context->currentEditingMeshIndex = meshIndex;
									m_Context->currentEditingTexIndex = t;
									m_Context->currentEditingFaceIndex = (int)i;
									m_OnEditTextureCubeCallback(path.string());
								}
								});

							if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", paths[i].c_str());
							ImGui::PopID();
						}
						ImGui::TreePop();
					}
					ImGui::EndGroup();
				}

				// --- Column 1: Action ---
				ImGui::TableSetColumnIndex(1);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.4f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
				if (ImGui::Button("X", { 24, 24 })) {
					textureIndexToRemove = static_cast<int>(t);
				}
				ImGui::PopStyleColor(2);
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("移除此纹理");

				// --- 纹理控制 ---
				bool& enableTexture = texture->GetEnable();
				if (ImGui::Checkbox(u8"", &enableTexture)) {
					texture->SetEnable(enableTexture);
					mesh->RemapMaterialTextures();
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
			if (m_Context->displayEntity) {
				auto shader = ShaderLibrary::Load("assets/shaders/Standard.glsl", {});
				auto model = std::make_shared<Model>(shader, path);
				m_Context->displayEntity.AddComponent<ModelComponent>(model);
			}
			});

		// 2. 替换现有纹理
		FileSelecter::Handle("EditTexture2D", [this](const std::string& path) {
			m_OnEditTexture2DCallback(path);
			});

		// 3. 加载新纹理
		//FileSelecter::Handle("LoadNewTexture", [this](const std::string& path) {
		//	m_OnCreateTextureCallback(path);
		//	});

		// 4. 替换整个 Cubemap
		FileSelecter::Handle("EditTextureCube", [this](const std::string& path) {
			m_OnEditTextureCubeCallback_Entirely(path);
			});

		// 5. 替换 Cubemap 的某一面
		FileSelecter::Handle("EditCubemapFace", [this](const std::string& path) {
			m_OnEditTextureCubeCallback(path);
			});

		// 6. 替换 Mesh 的 Shader
		FileSelecter::Handle("EditMeshShader", [this](const std::string& path) {
			m_OnMeshShaderFileOpenCallback(path);
			});

		// 7. 替换 Mesh 的 Shader
		FileSelecter::Handle("EditModelShader", [this](const std::string& path) {
			m_OnModelShaderFileOpenCallback(path);
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
						if (m_Context->displayEntity.HasAllofComponent<ModelComponent>()) return;

						if (type != PrimitiveType::Skybox) {
							auto defaultShader = ShaderLibrary::Load("assets/shaders/Standard.glsl", {});
							auto model = std::make_shared<Model>(type, defaultShader, std::vector<Refptr<Texture>>{});
							m_Context->displayEntity.AddComponent<ModelComponent>(model);
						}
						else {
							// Skybox 特殊处理
							auto skyShader = ShaderLibrary::Load("assets/shaders/TextureCube_Shader.glsl", {});
							std::vector<std::string> paths = {
								"assets/images/defaultSky/right.jpg", "assets/images/defaultSky/left.jpg",
								"assets/images/defaultSky/top.jpg",   "assets/images/defaultSky/bottom.jpg",
								"assets/images/defaultSky/front.jpg", "assets/images/defaultSky/back.jpg"
							};
							if (auto texture = TextureLibrary::Load("Default_Skybox_Cubemap", paths, TextureUsage::Cubemap)) {
								std::vector<Refptr<Texture>> texList = { texture };
								auto model = std::make_shared<Model>(type, skyShader, texList);
								m_Context->displayEntity.AddComponent<ModelComponent>(model);
								m_Context->displayEntity.RemoveComponent<TransformComponent>();
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
					FileSelecter::Open("ModelImportKey", "导入模型", "(.obj,.fbx,.gltf,.glb){.obj,.fbx,.gltf,.glb},.obj,.fbx,.gltf,.glb");
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
					static float s_InitIntensity = 1.0f;
					ImGui::TextDisabled("初始参数设置");

					ImGui::ColorEdit3("颜色", glm::value_ptr(s_InitColor));
					DrawVec3Control("方向", s_InitDir, glm::vec3(0.0f, -1.0f, 0.0f));
					ImGui::TextDisabled("光强参数");
					ImGui::DragFloat("光强", &s_InitIntensity, 0.01f, 0.0f, 10.0f);

					ImGui::Separator();

					// 实际执行添加的按钮
					if (ImGui::MenuItem("确认添加"))
					{
						if (m_Context->displayEntity && !m_Context->displayEntity.HasAllofComponent<DirectionalLightComponent>())
						{
							auto& dlc = m_Context->displayEntity.AddComponent<DirectionalLightComponent>();

							// 删除transform组件和model组件
							if(m_Context->displayEntity.HasAllofComponent<TransformComponent>())
								m_Context->displayEntity.RemoveComponent<TransformComponent>();
							if (m_Context->displayEntity.HasAllofComponent<ModelComponent>())
								m_Context->displayEntity.RemoveComponent<ModelComponent>();

							dlc.color = s_InitColor;
							dlc.direction = s_InitDir;

							dlc.intensity = s_InitIntensity;

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
					static float s_InitIntensity = 1.0f;
					ImGui::TextDisabled("初始参数设置");
					// 颜色选择面板
					ImGui::ColorEdit3("颜色", glm::value_ptr(s_InitColor));
					ImGui::TextDisabled("光强参数");
					ImGui::DragFloat("光强", &s_InitIntensity, 1.0f, 0.0f, 10000.0f);

					ImGui::Separator();

					// 实际执行添加的按钮
					if (ImGui::MenuItem("确认添加"))
					{
						if (m_Context->displayEntity && !m_Context->displayEntity.HasAllofComponent<PointLightComponent>())
						{
							auto& plc = m_Context->displayEntity.AddComponent<PointLightComponent>();
							plc.color = s_InitColor;

							plc.intensity = s_InitIntensity;

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
		ImGui::DragFloat("##X", &values.x, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		if (ImGui::Button("Y", buttonSize)) values.y = resetValue.y;
		ImGui::PopStyleColor(2);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.01f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		if (ImGui::Button("Z", buttonSize)) values.z = resetValue.z;
		ImGui::PopStyleColor(2);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.01f, 0.0f, 0.0f, "%.2f");
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