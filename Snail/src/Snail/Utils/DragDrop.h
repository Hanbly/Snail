#pragma once

#include <imgui.h>
#include <filesystem>
#include <functional>

namespace Snail {

	class DragDrop {
	public:
		// =======================================================
		// 拖拽源 (Source) 封装
		// type: 识别字符串 (如 "ASSETS_BROWSER_ITEM")
		// data: 要传递的数据
		// drawPreviewFn: 拖拽时鼠标旁边显示什么的绘制函数
		// =======================================================
		template<typename T>
		static void DrawDragDropSource(const std::string& type, const T& data, std::function<void()> drawPreviewFn = nullptr)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				// 自动计算大小并设置 Payload
				ImGui::SetDragDropPayload(type.c_str(), &data, sizeof(T));

				// 绘制预览图/文字
				if (drawPreviewFn)
					drawPreviewFn();
				else
					ImGui::Text("Moving..."); // 默认预览

				ImGui::EndDragDropSource();
			}
		}

		// 针对宽字符路径的特化重载 (因为 wchar_t* 长度是不定的，不能直接用 sizeof(T))
		inline static void DrawPathDragDropSource(const std::string& type, const std::filesystem::path& path, std::function<void()> drawPreviewFn = nullptr)
		{
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				const wchar_t* itemPathStr = path.c_str();
				size_t payloadSize = (wcslen(itemPathStr) + 1) * sizeof(wchar_t);
				ImGui::SetDragDropPayload(type.c_str(), itemPathStr, payloadSize);

				if (drawPreviewFn) drawPreviewFn();

				ImGui::EndDragDropSource();
			}
		}

		// =======================================================
		// 拖拽目标 (Target) 封装
		// type: 识别字符串
		// onDropFn: 当用户松手且数据有效时的回调函数
		// =======================================================
		template<typename T>
		static void DrawDragDropTarget(const std::string& type, std::function<void(const T&)> onDropFn)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str()))
				{
					// 将 void* 强转回 T 类型
					const T* data = (const T*)payload->Data;
					if (onDropFn) onDropFn(*data);
				}
				ImGui::EndDragDropTarget();
			}
		}

		// 针对宽字符路径的特化接收 (自动转回 std::filesystem::path)
		inline static void DrawPathDragDropTarget(const std::string& type, std::function<void(const std::filesystem::path&)> onDropFn)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(type.c_str()))
				{
					const wchar_t* pathStr = (const wchar_t*)payload->Data;
					if (onDropFn) onDropFn(std::filesystem::path(pathStr));
				}
				ImGui::EndDragDropTarget();
			}
		}

	};

}