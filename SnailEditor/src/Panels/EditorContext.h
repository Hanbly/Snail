#pragma once

#include "Snail.h"

namespace Snail {

	struct EditorContext {
		// 当前选中的实体
		Entity selectedEntity = {};
		// ImGuizmo 操作模式
		ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;
	};
}