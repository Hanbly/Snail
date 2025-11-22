#include "SNLpch.h"

#include "Snail.h"
#include "Layer/ExampleLayer.h"

class Example : public Snail::Application {
public:
	Example() {
		this->PushNorLayer(new ExampleLayer("example层", true));
		this->PushOverLayer(new Snail::ImGuiLayer("ImGui层", true));
	}
	~Example() {}

};

Snail::Application* Snail::CreateApplication()
{
	return new Example();
}