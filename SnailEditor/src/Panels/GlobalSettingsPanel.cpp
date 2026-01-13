#include "GlobalSettingsPanel.h"

namespace Snail{

	void GlobalSettingsPanel::Show(const Refptr<EditorCamera>& ec)
	{
		ImGui::Begin(u8"全局参数");

		// --- 性能监控 (Stats) ---
		ImGui::Text(u8"帧率:");
		ImGui::SameLine(60);
		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "%.1f FPS", ImGui::GetIO().Framerate);

		ImGui::Text(u8"帧时:");
		ImGui::SameLine(60);
		ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "%.3f ms", 1000.0f / ImGui::GetIO().Framerate);

		ImGui::Separator();

		// --- 渲染设置 ---
		ImGui::TextDisabled(u8"渲染管线");
		bool isInstancing = Renderer3D::GetEnableInstancing();
		if (ImGui::Checkbox(u8"启用实例化渲染 (Instancing)", &isInstancing)) {
			Renderer3D::SetEnableInstancing(isInstancing);
		}
		// 鼠标悬停提示
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip(u8"当开启时，相同的 Mesh 将合并为一个 DrawCall 进行批量绘制。\n大幅提升大量重复物体场景的性能。");
		}

		bool isUseIBL = Renderer3D::GetUseIBL();
		if (ImGui::Checkbox(u8"启用基于图像的光照 (IBL)", &isUseIBL)) {
			Renderer3D::SetUseIBL(isUseIBL);
		}
		// 鼠标悬停提示
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip(u8"当开启时，环境图像（天空盒）会被计算光照，并附加在PBR物体上。");
		}

		ImGui::Separator();


		// --- 相机设置 (Camera) ---
		if (ImGui::TreeNodeEx(u8"相机设置", ImGuiTreeNodeFlags_DefaultOpen))
		{
			glm::vec3 pos = ec->GetPosition();
			glm::vec3 rot = ec->GetRotation();

			ImGui::Text(u8"位置: %.2f,  %.2f,  %.2f", pos.x, pos.y, pos.z);
			ImGui::Text(u8"旋角: %.2f°, %.2f°, %.2f°", rot.x, rot.y, rot.z);

			const char* modeTypeStrings[] = { "None", "自由漫游（FPS）模式", "轨道球（Arcball）模式" };
			const char* currentModeString = modeTypeStrings[(int)ec->GetMode()];

			if (ImGui::BeginCombo(u8"控制模式", currentModeString))
			{
				for (int i = 0; i < 3; i++) // 遍历所有模式
				{
					bool isSelected = (currentModeString == modeTypeStrings[i]);
					if (ImGui::Selectable(modeTypeStrings[i], isSelected))
					{
						currentModeString = modeTypeStrings[i];
						ec->SetMode((EditorCameraMode)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::TreePop();
		}

		// --- gizmo ---
		if (ImGui::TreeNodeEx(u8"gizmo模式", ImGuiTreeNodeFlags_DefaultOpen))
		{

			ImVec4 activeColor = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];

			auto DrawToggleBtn = [&](const char* label, bool active) -> bool {
				if (active) ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
				bool clicked = ImGui::Button(label, ImVec2(60, 0)); // 固定宽度 60
				if (active) ImGui::PopStyleColor();
				return clicked;
				};

			// 操作类型 (Translate / Rotate / Scale)
			if (DrawToggleBtn(u8"位移", m_Context->mCurrentGizmoOperation == ImGuizmo::TRANSLATE)) {
				m_Context->mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			}
			ImGui::SameLine();
			if (DrawToggleBtn(u8"旋转", m_Context->mCurrentGizmoOperation == ImGuizmo::ROTATE)) {
				m_Context->mCurrentGizmoOperation = ImGuizmo::ROTATE;
			}
			ImGui::SameLine();
			if (DrawToggleBtn(u8"缩放", m_Context->mCurrentGizmoOperation == ImGuizmo::SCALE)) {
				m_Context->mCurrentGizmoOperation = ImGuizmo::SCALE;
			}
			// 坐标系模式 (Local / World)
			if (m_Context->mCurrentGizmoOperation != ImGuizmo::SCALE)
			{
				ImGui::Dummy(ImVec2(0, 5)); // 增加一点垂直间距

				if (DrawToggleBtn(u8"本地", m_Context->mCurrentGizmoMode == ImGuizmo::LOCAL)) {
					m_Context->mCurrentGizmoMode = ImGuizmo::LOCAL;
				}
				ImGui::SameLine();
				if (DrawToggleBtn(u8"世界", m_Context->mCurrentGizmoMode == ImGuizmo::WORLD)) {
					m_Context->mCurrentGizmoMode = ImGuizmo::WORLD;
				}
			}
			ImGui::TreePop();
		}

		// --- 伽马校正 ---
		if (m_Context->scene)
		{
			if (ImGui::TreeNodeEx(u8"伽马校正", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 获取引用的方式修改 Scene 中的数据
				auto& gamma = m_Context->scene->GetGamma();
				auto& exposure = m_Context->scene->GetExposure();

				ImGui::DragFloat("Gamma", &gamma, 0.1f, 1.0f, 3.0f, "%.1f");
				ImGui::DragFloat("Exposure", &exposure, 0.01f, 0.0f, 2.0f, "%.2f");

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}

}