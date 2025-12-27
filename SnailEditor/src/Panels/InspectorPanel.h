#pragma once

#include "Panel.h"
#include "Panels/EditorContext.h"
#include <glm/glm.hpp>

namespace Snail {

	// ---------------------------------------------------------
	// 属性面板 (Inspector)
	// 职责：显示选中实体的组件详情，处理属性修改和资源引用
	// ---------------------------------------------------------
	class InspectorPanel : public Panel {
	private:
		Refptr<Scene> m_Scene;
		Refptr<EditorContext> m_Context;

	public:
		InspectorPanel(const Refptr<Scene>& scene, const Refptr<EditorContext>& context)
			: m_Scene(scene), m_Context(context) {
		}

		void Show();

	private:
		// -------------------- 主流程 --------------------
		void DrawAllComponents(Entity entity);
		void DrawMultiSelection(const std::vector<Entity>& entities);
		void DrawAddComponentButton(Entity entity);
		void HandleFileDropCallbacks(); // 处理拖拽或文件选择回调

		// -------------------- 具体组件绘制 --------------------
		void DrawTagComponent(Entity entity);
		void DrawTransformComponent(Entity entity);
		void DrawDirectionalLightComponent(Entity entity);
		void DrawPointLightComponent(Entity entity);
		void DrawModelComponent(Entity entity);

		// -------------------- 模型组件辅助 --------------------
		// 绘制单个网格节点的UI
		void DrawMeshNode(size_t meshIndex, const Refptr<Mesh>& mesh);
		// 绘制纹理列表表格
		void DrawTextureTable(size_t meshIndex, const std::vector<Refptr<Texture>>& textures, Refptr<Mesh>& mesh);

		// -------------------- 通用 UI 控件 --------------------
		static void DrawVec3Control(const std::string& label, glm::vec3& values, glm::vec3& resetValue = glm::vec3(0.0f), float columnWidth = 100.0f);

		template<typename T, typename UIFunction>
		void DrawComponentWrapper(const std::string& name, Entity entity, UIFunction uiFunction);
	};

}