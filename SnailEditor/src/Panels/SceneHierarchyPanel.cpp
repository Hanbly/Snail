#include "SceneHierarchyPanel.h"

namespace Snail {

	void SceneHierarchyPanel::Show()
	{
		if (!m_Scene) { SNL_CORE_ERROR("SceneHierarchyPanel: 未定义的场景信息!"); return; };

		ImGui::Begin(u8"场景列表");

		// -------------------- 绘制实体列表 --------------------
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 4.0f });
		auto view = m_Scene->GetRegistry().view<entt::entity>();
		for (auto it = view.rbegin(), last = view.rend(); it != last; ++it) {
			Entity entity{ *it, m_Scene.get() };
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

		// -------------------- 空白处点击取消选中 --------------------
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			ResetSelectedEntity({});

		// ----------------- 在此延迟删除实体 --------------------
		if (m_Context->entityToDelete.IsValid())
			m_Scene->DestroyEntity(m_Context->entityToDelete);		

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawAddEntityPopup()
	{
		if (ImGui::BeginPopup("AddEntity"))
		{
			m_Scene->CreateEntity();
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

	void SceneHierarchyPanel::ResetSelectedEntity(const Entity& entity) {
		// 清除所有实体的描边状态
		auto modelview = m_Scene->GetAllofEntitiesWith<ModelComponent>();
		for (auto [e, model] : modelview.each())
			model.edgeEnable = false;

		m_Context->selectedEntity = entity;

		// 如果新选中的实体有模型，开启描边
		if (m_Context->selectedEntity && m_Context->selectedEntity.IsValid()) {
			if (m_Context->selectedEntity.HasAllofComponent<ModelComponent>()) {
				m_Context->selectedEntity.GetComponent<ModelComponent>().edgeEnable = true;
			}
		}
	}

	void SceneHierarchyPanel::AddSelectedEntity(const Entity& entity) {
		if (m_Context->selectedEntity == entity) return;

		// 这里目前的逻辑是替换选中，如果未来支持多选可修改此处
		m_Context->selectedEntity = {};

		if (entity && entity.IsValid()) {
			if (entity.HasAllofComponent<ModelComponent>()) {
				entity.GetComponent<ModelComponent>().edgeEnable = true;
			}
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		std::string name = "Unnamed Entity";
		if (entity.HasAllofComponent<TagComponent>())
			name = entity.GetComponent<TagComponent>().name;

		ImGuiTreeNodeFlags flags = ((m_Context->selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		// 转换为唯一指针ID
		uint32_t entityID = (uint32_t)(entt::entity)entity;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entityID, flags, name.c_str());

		if (ImGui::IsItemClicked())
		{
			ResetSelectedEntity(entity);
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
			if (m_Context->selectedEntity == entity)
				m_Context->selectedEntity = {};
			m_Context->entityToDelete = entity;
		}
	}
}