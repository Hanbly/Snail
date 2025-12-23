
#include "GlobalSettingsPanel.h"

namespace Snail{

	void GlobalSettingsPanel::Show(const Refptr<EditorCamera>& ec)
	{
		ImGui::Begin(u8"全局设置");

		// --- 性能 (Stats) ---
		ImGui::Text(u8"性能监控:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.1f FPS (%.3f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
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

		// --- 光源设置 (Lighting) ---
		if (m_Scene)
		{
			if (ImGui::TreeNodeEx(u8"场景光照", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 获取引用的方式修改 Scene 中的数据
				auto& ambient = m_Scene->GetAmbientStrength();

				ImGui::DragFloat("Ambient", &ambient, 0.001f, 0.0f, 1.0f, "%.2f");

				ImGui::TreePop();
			}
		}

		// --- gizmo 模式按钮 ---
		if (ImGui::RadioButton("Translate", m_Context->mCurrentGizmoOperation == ImGuizmo::TRANSLATE)) {
			m_Context->mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			ImGui::SameLine();
		}
		if (ImGui::RadioButton("Rotate", m_Context->mCurrentGizmoOperation == ImGuizmo::ROTATE)) {
			m_Context->mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
			ImGui::SameLine();
		}
		if (ImGui::RadioButton("Scale", m_Context->mCurrentGizmoOperation == ImGuizmo::SCALE)) {
			m_Context->mCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
		}
		if (m_Context->mCurrentGizmoOperation != ImGuizmo::SCALE) {
			if (ImGui::RadioButton("Local", m_Context->mCurrentGizmoMode == ImGuizmo::LOCAL)) {
				m_Context->mCurrentGizmoMode = ImGuizmo::MODE::LOCAL;
				ImGui::SameLine();
			}
			if (ImGui::RadioButton("World", m_Context->mCurrentGizmoMode == ImGuizmo::WORLD)) {
				m_Context->mCurrentGizmoMode = ImGuizmo::MODE::WORLD;
			}
		}

		ImGui::End();
	}

}