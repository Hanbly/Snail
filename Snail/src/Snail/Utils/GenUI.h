#pragma once

#include "Snail/Basic/Core.h"
#include "Snail/Basic/Macro.h"

#define IMGUI_DEFINE_MATH_OPERATORS // For ImGuiFileDialog
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"

namespace Snail {

	struct MenuNode {
		std::string Label;                      // 菜单显示的文字
		std::function<void()> Action = nullptr; // 点击后的逻辑（只有叶子节点需要）
		std::vector<MenuNode> SubMenus;         // 子菜单列表
		std::string Shortcut = "";              // 快捷键文本（可选）
		bool Enabled = true;                    // 是否可用

		// 构造函数：用于普通点击项
		MenuNode(const std::string& label, std::function<void()> action, const std::string& shortcut = "")
			: Label(label), Action(action), Shortcut(shortcut) {
		}

		// 构造函数：用于嵌套子菜单
		MenuNode(const std::string& label, std::vector<MenuNode> subMenus)
			: Label(label), SubMenus(subMenus) {
		}
	};

	class GenUI {
	public:
		static void MenuHierarchy(const std::vector<MenuNode>& nodes)
		{
			for (const auto& node : nodes)
			{
				if (!node.SubMenus.empty()) // 如果有子菜单，递归渲染
				{
					if (ImGui::BeginMenu(node.Label.c_str(), node.Enabled))
					{
						MenuHierarchy(node.SubMenus);
						ImGui::EndMenu();
					}
				}
				else // 如果是叶子节点，渲染 MenuItem
				{
					if (ImGui::MenuItem(node.Label.c_str(), node.Shortcut.empty() ? nullptr : node.Shortcut.c_str(), false, node.Enabled))
					{
						if (node.Action) node.Action();
					}
				}
			}
		}
	};

}