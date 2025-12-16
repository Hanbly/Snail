#pragma once

#include "Panel.h"

namespace Snail {

    // ==========================================================
    // 视口面板
    // ==========================================================
    class EditorViewportPanel : public Panel {
    private:
        glm::vec2 m_ViewportSize = {0, 0};
        glm::vec2 m_ViewportBounds[2] = { {0, 0}, {0, 0} }; // 左上角Min, 右下角Max (屏幕绝对坐标)
        bool m_ViewportHovered = false; // 鼠标是否悬停在视口上
        bool m_ViewportFocused = false; // 视口是否处于焦点
    public:
        EditorViewportPanel() {}

        void Show(const Refptr<FrameBuffer>& fbo, const Refptr<EditorCamera>& ec)
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
            if (isFocused && isOperating) {
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
                fbo->Resize((uint32_t)ImguiViewportSize.x, (uint32_t)ImguiViewportSize.y);
                m_ViewportSize.x = (uint32_t)ImguiViewportSize.x;
                m_ViewportSize.y = (uint32_t)ImguiViewportSize.y;

                ec->SetViewportSize((float)m_ViewportSize.x, (float)m_ViewportSize.y);
            }

            // 获取帧缓冲信息，绘制纹理
            uint32_t textureId = fbo->GetColorAttachment();
            ImGui::Image(
                (void*)(intptr_t)textureId, ImGui::GetContentRegionAvail(),
                ImVec2(0, 1), // UV0: 纹理的(0,1) -> 对应 OpenGL 左上
                ImVec2(1, 0)  // UV1: 纹理的(1,0) -> 对应 OpenGL 右下
            );

            ImGui::End();
            ImGui::PopStyleVar();
        }

        bool IsHovered() const { return m_ViewportHovered; }

        bool IsFocused() const { return m_ViewportFocused; }

        const glm::vec2& GetSize() const { return m_ViewportSize; }

        const glm::vec2& GetBoundMin() const { return m_ViewportBounds[0]; }
    };

}