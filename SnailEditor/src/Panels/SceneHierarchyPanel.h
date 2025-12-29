#pragma once

#include "Panel.h"
#include "Panels/EditorContext.h"

namespace Snail {

	// ---------------------------------------------------------
	// 场景层级面板 (Scene Hierarchy)
	// 职责：显示实体列表树，处理选中逻辑，添加/删除实体
	// ---------------------------------------------------------
	class SceneHierarchyPanel : public Panel {
	public:
		SceneHierarchyPanel(const Refptr<EditorContext>& context)
			: m_Context(context) {
		}

		void Show();

		// 回调类型
		using OnEntityFileOpenCallback = std::function<void(const std::string&)>;
		// 设置回调的函数
		void SetOnEntityFileOpenCallback(const OnEntityFileOpenCallback& callback) { m_OnEntityFileOpenCallback = callback; }
	private:
		// -------------------- UI 渲染辅助 --------------------
		void DrawEntityNode(Entity entity);
		void DrawAddEntityPopup();
	private:
		Refptr<EditorContext> m_Context;

		// 存储回调函数
		OnEntityFileOpenCallback m_OnEntityFileOpenCallback;
	};

}