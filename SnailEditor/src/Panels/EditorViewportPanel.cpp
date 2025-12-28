#include "EditorViewportPanel.h"

#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

namespace Snail{

	void EditorViewportPanel::Show(const Refptr<FrameBuffer>& fbo)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin(u8"离屏渲染视口");

		// ----------------- 处理鼠标坐标 ------------------
		// 获取视口范围和位置
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		// 将局部坐标转换为屏幕绝对坐标
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		// ---------------- 处理imgui事件 -------------------
		bool isHovered = ImGui::IsWindowHovered();
		bool isFocused = ImGui::IsWindowFocused();
		bool isOperating = ImGui::IsMouseDown(ImGuiMouseButton_Right) || ImGui::IsMouseDown(ImGuiMouseButton_Middle);
		if (isFocused && isOperating) { // 从视口内拖拽鼠标到视口外的情况
			isHovered = true; // 强制保持 Hover 状态，防止拦截开启
		}
		m_ViewportHovered = isHovered;
		m_ViewportFocused = isFocused;
		// 如果鼠标悬停，且点击了任意键，强制设置 ImGui 焦点到当前窗口
		if (m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
			m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right) ||
			m_ViewportHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {

			ImGui::SetWindowFocus();
		}
		// !(m_ViewportHovered || m_ViewportFocused) 与 !m_ViewportHovered && !m_ViewportFocused : (鼠标 悬浮在视口 || 聚焦在视口) 都会让imgui忽略鼠标事件，从而让视口内容接收鼠标事件；不知道为什么键盘事件始终无法被imgui捕获，不论视口是什么状态。
		// 目标是：鼠标 一旦悬浮在视口 imgui就忽略鼠标事件；一旦聚焦于视口 imgui就忽略键盘事件；当鼠标悬浮在视口以外其它部分时，imgui应该拦截鼠标事件；当聚焦于视口以外其它部分时，imgui应该拦截键盘事件。
		Application::Get().GetImGuiLayer()->BlockMouseEvents(!m_ViewportHovered);
		Application::Get().GetImGuiLayer()->BlockKeyEvents(!m_ViewportFocused);
		//SNL_CORE_WARN("Hovered {0}", m_ViewportHovered);
		//SNL_CORE_WARN("Focused {0}", m_ViewportFocused);

		// ---------------- 处理resize ------------------
		ImVec2 ImguiViewportSize = ImGui::GetContentRegionAvail();
		ImGui::Text("视口大小: %.0f x %.0f", ImguiViewportSize.x, ImguiViewportSize.y);
		if ((m_ViewportSize != *(glm::vec2*)&ImguiViewportSize) && ImguiViewportSize.x > 0 && ImguiViewportSize.y > 0) {
			m_ViewportSize.x = ImguiViewportSize.x;
			m_ViewportSize.y = ImguiViewportSize.y;

			fbo->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera->SetViewportSize((float)m_ViewportSize.x, (float)m_ViewportSize.y);
		}

		// ----------------- 获取帧缓冲信息，绘制纹理 -----------------------
		uint32_t textureId = fbo->GetColorAttachment();
		//uint32_t textureId = fbo->GetMaskAttachment();
		ImGui::Image(
			(void*)(intptr_t)textureId, ImGui::GetContentRegionAvail(),
			ImVec2(0, 1), // UV0: 纹理的(0,1) -> 对应 OpenGL 左上
			ImVec2(1, 0)  // UV1: 纹理的(1,0) -> 对应 OpenGL 右下
		);

		// --------------- 绘制gizmo ------------------
		ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportSize.x, m_ViewportSize.y);

		ImGuizmo::SetDrawlist(); // 如果在一个 ImGui 窗口内绘制，确保 ImGuizmo 使用该窗口的绘图列表

		// 收集选中物体并计算中心点
		std::vector<Entity> selectedEntities;
		glm::vec3 avgPosition = { 0.0f, 0.0f, 0.0f };
		int count = 0;
		auto modelview = m_Scene->GetAllofEntitiesWith<TransformComponent, ModelComponent>();
		for (auto [entity, transform, model] : modelview.each()) {
			if (model.edgeEnable) {
				selectedEntities.push_back({ entity, m_Scene.get() });
				avgPosition += transform.position;
				count++;
			}
		}

		// 如果没有选中任何物体，直接结束
		if (selectedEntities.empty()) {
			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}

		// 计算中心位置
		avgPosition /= (float)count; // 坐标总和除以数量就是中心坐标
		// 将计算出的中心点和共有属性同步到 Context
		m_Context->entitiesPosition = avgPosition;
		// 旋转和缩放取第一个物体的作为基准显示
		m_Context->entitiesRotation = selectedEntities[0].GetComponent<TransformComponent>().rotation;
		m_Context->entitiesScale = selectedEntities[0].GetComponent<TransformComponent>().scale;

		// ImGuizmo::RecomposeMatrixFromComponents 构建 Gizmo 矩阵 ref
		glm::mat4 ref;
		TransformComponent firstTransform = selectedEntities[0].GetComponent<TransformComponent>();

		if (selectedEntities.size() == 1) {
			// 单选模式：完全对齐物体
			ImGuizmo::RecomposeMatrixFromComponents(
				glm::value_ptr(firstTransform.position),
				glm::value_ptr(firstTransform.rotation),
				glm::value_ptr(firstTransform.scale),
				glm::value_ptr(ref)
			);
		}
		else {
			// 多选模式：位置在中心
			ImGuizmo::RecomposeMatrixFromComponents(
				glm::value_ptr(m_Context->entitiesPosition),
				glm::value_ptr(m_Context->entitiesRotation),
				glm::value_ptr(m_Context->entitiesScale),
				glm::value_ptr(ref)
			);
		}
		// 绘制 Gizmo
		ImGuizmo::SetID(-1);

		// 备份旧矩阵用于计算 Delta
		glm::mat4 deltaMatrix = glm::mat4(1.0f);
		glm::mat4 oldRef = ref;

		ImGuizmo::Manipulate(
			glm::value_ptr(m_EditorCamera->GetViewMatrix()),
			glm::value_ptr(m_EditorCamera->GetProjection()),
			m_Context->mCurrentGizmoOperation,
			m_Context->mCurrentGizmoMode,
			glm::value_ptr(ref)
		);


		// 如果操作了，计算 Delta 并应用到所有物体
		if (ImGuizmo::IsUsing()) {
			// 计算变化量： 新 = Delta * 旧  ==>  Delta = 新 * Inverse(旧)
			// 这种方法能同时处理 平移、旋转和缩放
			deltaMatrix = ref * glm::inverse(oldRef);

			for (auto& entity : selectedEntities) {
				auto& transform = entity.GetComponent<TransformComponent>();

				// A. 还原该物体当前的矩阵
				glm::mat4 entityMatrix;
				ImGuizmo::RecomposeMatrixFromComponents(
					glm::value_ptr(transform.position),
					glm::value_ptr(transform.rotation),
					glm::value_ptr(transform.scale),
					glm::value_ptr(entityMatrix)
				);

				// B. 应用变化量
				glm::mat4 newEntityMatrix = deltaMatrix * entityMatrix;

				// C. 分解回组件
				float translation[3], rotation[3], scale[3];
				ImGuizmo::DecomposeMatrixToComponents(
					glm::value_ptr(newEntityMatrix),
					translation,
					rotation,
					scale
				);

				transform.position = glm::make_vec3(translation);
				transform.rotation = glm::make_vec3(rotation);
				transform.scale = glm::make_vec3(scale);
			}
		}


		ImGui::End();
		ImGui::PopStyleVar();
	}

}