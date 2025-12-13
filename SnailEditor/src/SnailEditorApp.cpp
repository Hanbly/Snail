#include "SNLpch.h"

#include "Snail.h"
#include "Layer/SnailEditorLayer.h"

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