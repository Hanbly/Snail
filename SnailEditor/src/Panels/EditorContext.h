#pragma once

#include "Snail.h"

namespace Snail {

	struct EditorContext {
		// 场景
		Refptr<Scene> scene;		
		// 当前展示实体和选中实体列表
		Entity displayEntity = {};
		std::vector<Entity> selectedEntities = {};
		// 要删除的实体
		std::vector<Entity> entitiesToDelete = {};
		// 多选物体时的共有属性
		glm::vec3 entitiesPosition = glm::vec3(0.0f);
		glm::vec3 entitiesRotation = glm::vec3(0.0f);
		glm::vec3 entitiesScale = glm::vec3(1.0f);
		// ImGuizmo 操作模式
		ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;

		// 当前正在编辑的 mesh 和 texture 索引
		size_t currentEditingMeshIndex = 0;
		size_t currentEditingTexIndex = 0;
		int    currentEditingFaceIndex = 0; // 0-5 for cubemaps

		TextureUsage pendingTextureUsage = TextureUsage::None;


		// -------------------- 选中状态管理 --------------------

		bool IsEntitySelected(Entity entity) const {
			for (const auto& e : selectedEntities)
				if (entity == e) return true;
			return false;
		}

		// 清除所有实体的描边状态
		void ClearSelectedEntities() {
			auto modelview = scene->GetAllofEntitiesWith<ModelComponent>();
			for (auto [e, model] : modelview.each())
				model.edgeEnable = false;
			displayEntity = {};
			selectedEntities.clear();
		}

		// 设置选中实体（并处理高亮/描边逻辑）
		void EditorContext::ResetSelectedEntity(const Entity& entity) {

			ClearSelectedEntities();

			// 如果新选中的实体有模型，开启描边
			if (entity && entity.IsValid()) {
				if (entity.HasAllofComponent<ModelComponent>()) {
					entity.GetComponent<ModelComponent>().edgeEnable = true;
				}
			}

			displayEntity = entity;
			selectedEntities.push_back(entity);
		}

		// 追加选中实体（用于多选或Ctrl点击逻辑）
		void EditorContext::AddSelectedEntity(const Entity& entity) {
			if (IsEntitySelected(entity)) {
				if (entity.HasAllofComponent<ModelComponent>()) {
					entity.GetComponent<ModelComponent>().edgeEnable = true;
				}
				return;
			}

			if (entity && entity.IsValid()) {
				if (entity.HasAllofComponent<ModelComponent>()) {
					entity.GetComponent<ModelComponent>().edgeEnable = true;
				}
			}

			displayEntity = {};
			selectedEntities.push_back(entity);
		}
	};
}