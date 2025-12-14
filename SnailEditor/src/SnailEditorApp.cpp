#include "Layer/SnailEditorLayer.h"

// 程序入口 -------------------------
#include "Snail/Basic/EntryPoint.h"
//----------------------------------

class SnailEditor : public Snail::Application {
public:
	SnailEditor() {
		this->PushNorLayer(new Snail::SnailEditorLayer("SnailEditorLayer 层", true));
	}
	~SnailEditor() = default;
};

Snail::Application* Snail::CreateApplication()
{
	return new SnailEditor();
}