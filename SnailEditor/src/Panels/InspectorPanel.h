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
	public:
		InspectorPanel(const Refptr<EditorContext>& context);

		void Show();

		// 回调类型
		using OnEditTexture2DCallback = std::function<void(const std::string&)>;
		using OnEditTextureCubeCallback = std::function<void(const std::string&)>;
		using OnEditTextureCubeCallback_Entirely = std::function<void(const std::string&)>;
		using OnCreateTextureCallback = std::function<void(const std::string&)>;
		using OnMeshShaderFileOpenCallback = std::function<void(const std::string&)>;
		using OnModelShaderFileOpenCallback = std::function<void(const std::string&)>;
		// 设置回调的函数
		void SetEditTexture2DCallback(const OnEditTexture2DCallback& callback) { m_OnEditTexture2DCallback = callback; }
		void SetEditTextureCubeCallback(const OnEditTextureCubeCallback& callback) { m_OnEditTextureCubeCallback = callback; }
		void SetEditTextureCubeCallback_Entirely(const OnEditTextureCubeCallback_Entirely& callback) { m_OnEditTextureCubeCallback_Entirely = callback; }
		void SetCreateTextureCallback(const OnCreateTextureCallback& callback) { m_OnCreateTextureCallback = callback; }
		void SetMeshShaderFileOpenCallback(const OnMeshShaderFileOpenCallback& callback) { m_OnMeshShaderFileOpenCallback = callback; }
		void SetModelShaderFileOpenCallback(const OnModelShaderFileOpenCallback& callback) { m_OnModelShaderFileOpenCallback = callback; }
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
	private:
		Refptr<EditorContext> m_Context;

		// 存储回调函数
		OnEditTexture2DCallback m_OnEditTexture2DCallback;
		OnEditTextureCubeCallback m_OnEditTextureCubeCallback;
		OnEditTextureCubeCallback_Entirely m_OnEditTextureCubeCallback_Entirely;
		OnCreateTextureCallback m_OnCreateTextureCallback;
		OnMeshShaderFileOpenCallback m_OnMeshShaderFileOpenCallback;
		OnModelShaderFileOpenCallback m_OnModelShaderFileOpenCallback;
	};

}