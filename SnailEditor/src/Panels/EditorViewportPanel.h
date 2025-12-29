#pragma once

#include "Panel.h"
#include "Panels/EditorContext.h"

#include <glm/glm.hpp>

namespace Snail {

    // ==========================================================
    // 视口面板
    // ==========================================================
	class EditorViewportPanel : public Panel {
	public:
		EditorViewportPanel(const Refptr<EditorCamera>& ec, const Refptr<EditorContext>& context)
			: m_EditorCamera(ec), m_Context(context) {
		}

		void Show(const Refptr<FrameBuffer>& fbo);

		bool IsHovered() const { return m_ViewportHovered; }
		bool IsFocused() const { return m_ViewportFocused; }
		const glm::vec2& GetSize() const { return m_ViewportSize; }
		const glm::vec2& GetBoundMin() const { return m_ViewportBounds[0]; }

		// 回调类型
		using OnSceneFileOpenCallback = std::function<void(const std::string&)>;
		using OnEntityFileOpenCallback = std::function<void(const std::string&)>;
		// 设置回调的函数
		void SetOnSceneFileOpenCallback(const OnSceneFileOpenCallback& callback) { m_OnSceneFileOpenCallback = callback; }
		void SetOnEntityFileOpenCallback(const OnEntityFileOpenCallback& callback) { m_OnEntityFileOpenCallback = callback; }
	private:
        Refptr<EditorCamera> m_EditorCamera;
        Refptr<EditorContext> m_Context;

        glm::vec2 m_ViewportSize = {0, 0};
        glm::vec2 m_ViewportBounds[2] = { {0, 0}, {0, 0} }; // 左上角Min, 右下角Max (屏幕绝对坐标)
        bool m_ViewportHovered = false; // 鼠标是否悬停在视口上
        bool m_ViewportFocused = false; // 视口是否处于焦点

		// 存储回调函数
		OnSceneFileOpenCallback m_OnSceneFileOpenCallback;
		OnEntityFileOpenCallback m_OnEntityFileOpenCallback;
    };

}