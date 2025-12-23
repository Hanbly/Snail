#pragma once

#include "Panel.h"
#include "Panels/EditorContext.h"

namespace Snail {

    // ==========================================================
    // 场景（的实体）列表 (Scene Hierarchy) & 属性面板 (Properties)
    // ==========================================================
    class SceneHierarchyPanel : public Panel {
    private:
        Refptr<Scene> m_Scene;
        Refptr<EditorContext> m_Context;
    public:
		SceneHierarchyPanel(const Refptr<Scene>& scene, const Refptr<EditorContext>& context)
			: m_Scene(scene), m_Context(context) {}

        void Show();

        void ResetSelectedEntity(const Entity& entity);
        void AddSelectedEntity(const Entity& entity);

    private: // -------------------- UI ------------------------
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
        static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

        template<typename T, typename UIFunction>
        static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction);
    };

}