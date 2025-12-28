#include "AssetsBrowserPanel.h"

namespace Snail {

	// 基础资源路径
	static const std::filesystem::path s_AssetsDirectory = "assets";

	AssetsBrowserPanel::AssetsBrowserPanel()
		: m_CurrentDirectory(s_AssetsDirectory)
	{
	}

	void AssetsBrowserPanel::LoadIcons()
	{
		m_DirectoryIcon = TextureLibrary::Load("AssetsBrowser_Icons_DirectoryIcon", { "resources/icons/DirectoryIcon.png" }, TextureUsage::None);
		m_DirectoryBackIcon = TextureLibrary::Load("AssetsBrowser_Icons_DirectoryBackIcon", { "resources/icons/DirectoryBackIcon.png" }, TextureUsage::None);

		m_FileIcon = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon", { "resources/icons/FileIcon.png" }, TextureUsage::None);

		m_FileIcon_snl = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_snl", { "resources/icons/FileIcon_snl.png" }, TextureUsage::None);
		m_FileIcon_txt = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_txt", { "resources/icons/FileIcon_txt.png" }, TextureUsage::None);
		m_FileIcon_md = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_md", { "resources/icons/FileIcon_md.png" }, TextureUsage::None);

		m_FileIcon_png = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_png", { "resources/icons/FileIcon_png.png" }, TextureUsage::None);
		m_FileIcon_jpg = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_jpg", { "resources/icons/FileIcon_jpg.png" }, TextureUsage::None);

		m_FileIcon_obj = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_obj", { "resources/icons/FileIcon_obj.png" }, TextureUsage::None);
		m_FileIcon_mtl = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_mtl", { "resources/icons/FileIcon_mtl.png" }, TextureUsage::None);
		m_FileIcon_fbx = TextureLibrary::Load("AssetsBrowser_Icons_FileIcon_fbx", { "resources/icons/FileIcon_fbx.png" }, TextureUsage::None);
	}

	void AssetsBrowserPanel::show()
	{
		ImGui::Begin(u8"资源管理器");

		// ---------------------------------------------------------
		// 1. 顶部工具栏：返回按钮 + 当前路径
		// ---------------------------------------------------------

		// 判断是否在根目录
		bool isRoot = (m_CurrentDirectory == std::filesystem::path(s_AssetsDirectory));

		// 禁用状态下绘制返回按钮
		if (isRoot) ImGui::BeginDisabled();
		{
			uint32_t backTexId = m_DirectoryBackIcon ? m_DirectoryBackIcon->GetRendererId() : 0;
			if (ImGui::ImageButton("##backBtn", (ImTextureID)(uint64_t)backTexId, { 24, 24 }, { 0, 1 }, { 1, 0 }))
			{
				if (!isRoot)
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		if (isRoot) ImGui::EndDisabled();

		// 在按钮旁显示当前路径
		ImGui::SameLine();

		// 垂直居中文本
		float textOffsetY = (ImGui::GetStyle().FramePadding.y + 24.0f - ImGui::GetFontSize()) * 0.5f;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + textOffsetY);

		// 计算相对路径用于显示
		std::filesystem::path relativePath = std::filesystem::relative(m_CurrentDirectory, s_AssetsDirectory);
		std::string pathStr = relativePath.u8string() == "." ? s_AssetsDirectory.u8string() : s_AssetsDirectory.u8string() + "\\" + relativePath.u8string();
		ImGui::Text(pathStr.empty() ? "assets" : pathStr.c_str());

		ImGui::Separator();

		// ---------------------------------------------------------
		// 2. 内容网格布局
		// ---------------------------------------------------------

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		// ---------------------------------------------------------
		// 3. 遍历文件与绘制
		// ---------------------------------------------------------

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().u8string();

			ImGui::PushID(filenameString.c_str());

			bool isDirectory = directoryEntry.is_directory();
			Refptr<Texture> icon = isDirectory ? m_DirectoryIcon : m_FileIcon;

			if (icon == m_FileIcon) {
				std::string extension = path.extension().string();
				if (extension == ".snl")		icon = m_FileIcon_snl;
				else if (extension == ".txt")	icon = m_FileIcon_txt;
				else if (extension == ".md")	icon = m_FileIcon_md;

				else if (extension == ".png")	icon = m_FileIcon_png;
				else if (extension == ".jpg")	icon = m_FileIcon_jpg;

				else if (extension == ".obj")	icon = m_FileIcon_obj;
				else if (extension == ".mtl")	icon = m_FileIcon_mtl;
				else if (extension == ".fbx")	icon = m_FileIcon_fbx;
			}

			uint32_t textureId = icon ? icon->GetRendererId() : 0;

			// 绘制透明背景的图标按钮
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton("##browserBtn", (ImTextureID)(uint64_t)textureId, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			// 拖拽源 (Drag Source)
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPathStr = path.c_str();
				size_t payloadSize = (wcslen(itemPathStr) + 1) * sizeof(wchar_t);
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPathStr, payloadSize);

				ImGui::ImageButton("##dragImg", (ImTextureID)(uint64_t)textureId, { 64, 64 }, { 0, 1 }, { 1, 0 });
				ImGui::Text(filenameString.c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			// 双击交互
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isDirectory)
					m_CurrentDirectory /= path.filename();
				else
				{
					// TODO: 打开文件逻辑
					std::string extension = path.extension().string();

					// --- 场景文件 ---
					if (extension == ".snl" && m_OnSceneFileOpenCallback)
					{
						// 注意：这里传出去的是绝对路径或相对路径的 string
						m_OnSceneFileOpenCallback(path.string());
					}

				}
			}

			// 文件名显示
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::End();
	}
}