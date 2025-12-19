#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#define IMGUI_DEFINE_MATH_OPERATORS // For ImGuiFileDialog
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"

namespace Snail {

	class FileSelecter {
	public:
		// 封装打开对话框的逻辑
		static void Open(const std::string& key, const std::string& title, const std::string& filters) {
			IGFD::FileDialogConfig config;
			config.path = ".";

			ImGuiFileDialog::Instance()->OpenDialog(key, title, filters.c_str(), config);
		}

		// 封装处理结果的逻辑
		static void Handle(const std::string& key, std::function<void(const std::string&)> onConfirm) {
			if (ImGuiFileDialog::Instance()->Display(key, ImGuiWindowFlags_NoCollapse, { 750.0f, 500.0f })) {
				if (ImGuiFileDialog::Instance()->IsOk()) {
					std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
					onConfirm(filePath); // 执行传入的回调逻辑
				}
				ImGuiFileDialog::Instance()->Close();
			}
		}
	};

}
