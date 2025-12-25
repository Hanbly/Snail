#pragma once

#include "Panel.h"
#include "Panels/EditorContext.h"

namespace Snail {

	// ---------------------------------------------------------
	// 场景层级面板 (Scene Hierarchy)
	// 职责：显示实体列表树，处理选中逻辑，添加/删除实体
	// ---------------------------------------------------------
	class SceneHierarchyPanel : public Panel {
	private:
		Refptr<Scene> m_Scene;
		Refptr<EditorContext> m_Context;

	public:
		SceneHierarchyPanel(const Refptr<Scene>& scene, const Refptr<EditorContext>& context)
			: m_Scene(scene), m_Context(context) {
		}

		void Show();

		// -------------------- 选中状态管理 --------------------
		// 设置选中实体（并处理高亮/描边逻辑）
		void ResetSelectedEntity(const Entity& entity);
		// 追加选中实体（用于多选或Ctrl点击逻辑）
		void AddSelectedEntity(const Entity& entity);

	private:
		// -------------------- UI 渲染辅助 --------------------
		void DrawEntityNode(Entity entity);
		void DrawAddEntityPopup();
	};

}