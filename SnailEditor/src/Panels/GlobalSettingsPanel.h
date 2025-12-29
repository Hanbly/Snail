#pragma once

#include "Panel.h"
#include "Panels/EditorContext.h"

namespace Snail {

    // ==========================================================
    // 全局设置 (Environment & Stats)
    // ==========================================================
    class GlobalSettingsPanel : public Panel {
    private:
        Refptr<EditorContext> m_Context;
    public:
        GlobalSettingsPanel(const Refptr<EditorContext>& context)
            : m_Context(context) {}

        void Show(const Refptr<EditorCamera>& ec);
    };

}