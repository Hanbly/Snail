#include "EditorContext.h"

namespace Snail {

	void EditorContext::ResetSelectedEntity(const Entity& entity) {
		// 清除所有实体的描边状态
		auto modelview = scene->GetAllofEntitiesWith<ModelComponent>();
		for (auto [e, model] : modelview.each())
			model.edgeEnable = false;

		selectedEntity = entity;

		// 如果新选中的实体有模型，开启描边
		if (selectedEntity && selectedEntity.IsValid()) {
			if (selectedEntity.HasAllofComponent<ModelComponent>()) {
				selectedEntity.GetComponent<ModelComponent>().edgeEnable = true;
			}
		}
	}

	void EditorContext::AddSelectedEntity(const Entity& entity) {
		if (selectedEntity == entity) return;

		selectedEntity = {};

		if (entity && entity.IsValid()) {
			if (entity.HasAllofComponent<ModelComponent>()) {
				entity.GetComponent<ModelComponent>().edgeEnable = true;
			}
		}
	}

}