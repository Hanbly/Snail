#include "SceneHierarchyPanel.h"

namespace Snail {

	void SceneHierarchyPanel::Show()
	{
		if (!m_Context->scene) { SNL_CORE_ERROR("SceneHierarchyPanel: 未定义的场景信息!"); return; };

		ImGui::Begin(u8"场景列表");

		// -------------------- 绘制实体列表 --------------------
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 4.0f });
		auto view = m_Context->scene->GetRegistry().view<entt::entity>();
		for (auto it = view.rbegin(), last = view.rend(); it != last; ++it) {
			Entity entity{ *it, m_Context->scene.get() };
			DrawEntityNode(entity);
		}
		ImGui::PopStyleVar();

		// -------------------- 底部功能区 --------------------
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::PushItemWidth(-1);
		if (ImGui::Button("添加实体", { -1, 30 }))
			ImGui::OpenPopup("AddEntity");

		DrawAddEntityPopup();
		ImGui::PopItemWidth();

		// ---------------- 处理作为拖拽目标的逻辑 -------------------
		DragDrop::DrawPathDragDropTarget("ASSETS_BROWSER_ITEM", [&](const std::filesystem::path& path) {
			std::string extension = path.extension().string();

			if (extension == ".obj" || extension == ".fbx")
			{
				m_OnEntityFileOpenCallback(path.string());
			}
			});

		// -------------------- 空白处点击取消选中 --------------------
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
		{
			// 只有不按住 Ctrl 点击空白处,取消选中
			if(!Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL))
				m_Context->ClearSelectedEntities();
		}
			

		// ----------------- 在此延迟删除实体 --------------------
		if (m_Context->entitiesToDelete.size()) {
			for (auto& etd : m_Context->entitiesToDelete) {
				if (etd.IsValid()) m_Context->scene->DestroyEntity(etd);
			}
			m_Context->selectedEntities.clear();
			m_Context->entitiesToDelete.clear();
		}		

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawAddEntityPopup()
	{
		if (ImGui::BeginPopup("AddEntity"))
		{
			m_Context->scene->CreateEntity();
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		std::string name = "Unnamed Entity";
		if (entity.HasAllofComponent<TagComponent>())
			name = entity.GetComponent<TagComponent>().name;

		bool isSelected = m_Context->IsEntitySelected(entity);

		ImGuiTreeNodeFlags flags = (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		// 转换为唯一指针ID
		uint32_t entityID = (uint32_t)(entt::entity)entity;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entityID, flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			bool ctrlPressed = Input::IsKeyPressed(SNL_KEY_LEFT_CONTROL);
			if (ctrlPressed)
			{
				// 如果按住 Ctrl：执行多选逻辑
				if (!isSelected) {
					// 如果没选中，则添加到选中列表
					m_Context->AddSelectedEntity(entity);
				}
			}
			else
			{
				// 如果没按 Ctrl：执行单选逻辑
				m_Context->ResetSelectedEntity(entity);
			}
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("删除实体"))
				entityDeleted = true;
			ImGui::EndPopup();
		}

		if (opened) ImGui::TreePop();

		if (entityDeleted)
		{
			if (isSelected)
				m_Context->displayEntity = {};
			m_Context->entitiesToDelete.reserve(m_Context->selectedEntities.size());
			m_Context->entitiesToDelete.insert(m_Context->entitiesToDelete.end(), m_Context->selectedEntities.begin(), m_Context->selectedEntities.end());
		}
	}
}