#pragma once

#include "Snail.h"

#include "Snail/Utils/GenUI.h"
#include "Snail/Utils/FileSelecter.h"
#include "Snail/Scene/SceneUtils.h"

#include "Panels/EditorContext.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/EditorViewportPanel.h"
#include "Panels/GlobalSettingsPanel.h"

#include <filesystem>
#include <glm/glm.hpp>

namespace Snail {

    class SnailEditorLayer : public Layer
    {
    private:
        // -------------------临时------------------------------------------

        Refptr<FrameBuffer> m_FBO;
		Refptr<VertexArray> m_ScreenQuadVAO; // 屏幕四边形
		glm::vec3 m_OutlineColor = { 1.0f, 0.5f, 0.0f }; // 默认橙色
		int m_OutlineWidth = 3;

        // ECS 核心
        Refptr<Scene> m_Scene;

        // 编辑器特有
        Refptr<EditorCamera> m_EditorCamera;
        Refptr<EditorContext> m_EditorContext;

        SceneHierarchyPanel m_SHpanel;
        EditorViewportPanel m_EVpanel;
        GlobalSettingsPanel m_GSpanel;

		// 初始化一个覆盖 -1 到 1 范围的平面
		void InitScreenQuad()
		{
			float quadVertices[] = {
				// 坐标(x,y)   // 纹理坐标(u,v)
				-1.0f,  1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f,  0.0f, 0.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,
				 1.0f,  1.0f,  1.0f, 1.0f
			};
			uint32_t quadIndices[] = { 0, 1, 2, 2, 3, 0 };

			m_ScreenQuadVAO = VertexArray::Create();
			auto vbo = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
			vbo->SetLayout(BufferLayout::Create({
				{ "a_Pos",       VertexDataType::Float2 },
				{ "a_TexCoords", VertexDataType::Float2 }
				}));
			m_ScreenQuadVAO->SetVertexBuffer(vbo);
			m_ScreenQuadVAO->SetIndexBuffer(IndexBuffer::Create(quadIndices, sizeof(quadIndices)));
		}

        //------------------------------------------------------------------
    public:
		SnailEditorLayer(const std::string& layerName, const bool& layerEnabled);

        virtual void OnAttach() override;
        virtual void OnDetach() override {}

        virtual void OnUpdate(const Timestep& ts) override;

        virtual void OnEvent(Event& e) override;

        virtual void OnRender() override;
        virtual void OnImGuiRender() override;
	private:
		bool OnMousePressed(MousePressEvent& e);
    };

}
