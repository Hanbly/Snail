#pragma once

#include "Panel.h"
#include "Panels/EditorContext.h"

#include <glm/glm.hpp>

namespace Snail {

    // ==========================================================
    // 视口面板
    // ==========================================================
    class EditorViewportPanel : public Panel {
	private:
		Refptr<Scene> m_Scene;
        Refptr<EditorCamera> m_EditorCamera;
        Refptr<EditorContext> m_Context;

        glm::vec2 m_ViewportSize = {0, 0};
        glm::vec2 m_ViewportBounds[2] = { {0, 0}, {0, 0} }; // 左上角Min, 右下角Max (屏幕绝对坐标)
        bool m_ViewportHovered = false; // 鼠标是否悬停在视口上
        bool m_ViewportFocused = false; // 视口是否处于焦点
    public:
		EditorViewportPanel(const Refptr<Scene>& scene, const Refptr<EditorCamera>& ec, const Refptr<EditorContext>& context)
			: m_Scene(scene), m_EditorCamera(ec), m_Context(context) {}

        void Show(const Refptr<FrameBuffer>& fbo);

        bool IsHovered() const { return m_ViewportHovered; }
        bool IsFocused() const { return m_ViewportFocused; }
        const glm::vec2& GetSize() const { return m_ViewportSize; }
        const glm::vec2& GetBoundMin() const { return m_ViewportBounds[0]; }
    };

}