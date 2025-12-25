#pragma once

#include "Snail.h"

namespace Snail {

	struct EditorContext {
		// 当前选中的实体
		Entity selectedEntity = {};
		// 是否删除实体
		Entity entityToDelete = {};
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
	};
}