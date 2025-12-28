#pragma once

#include "Snail.h"

#include "Panel.h"
#include "Panels/EditorContext.h"

#include <filesystem>
#include <functional>

namespace Snail {

	class AssetsBrowserPanel {
	public:
		AssetsBrowserPanel();

		void LoadIcons();
		void show();

		// 回调类型
		using OnSceneFileOpenCallback = std::function<void(const std::string&)>;
		// 设置回调的函数
		void SetOnSceneFileOpenCallback(const OnSceneFileOpenCallback& callback) { m_OnSceneFileOpenCallback = callback; }

	private:
		std::filesystem::path m_CurrentDirectory;

		Refptr<Texture> m_DirectoryIcon;
		Refptr<Texture> m_DirectoryBackIcon;
		Refptr<Texture> m_FileIcon;

		Refptr<Texture> m_FileIcon_snl;
		Refptr<Texture> m_FileIcon_txt;
		Refptr<Texture> m_FileIcon_md;

		Refptr<Texture> m_FileIcon_png;
		Refptr<Texture> m_FileIcon_jpg;

		Refptr<Texture> m_FileIcon_obj;
		Refptr<Texture> m_FileIcon_mtl;
		Refptr<Texture> m_FileIcon_fbx;

		// 存储回调函数
		OnSceneFileOpenCallback m_OnSceneFileOpenCallback;
	
	private:

	};

}