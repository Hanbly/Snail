#include "SNLpch.h"

#include "Snail.h"
#include "Layer/ExampleLayer.h"

class Example : public Snail::Application {
public:
	Example() {
		this->PushNorLayer(new ExampleLayer("example层"));
	}
	~Example() {}

};

Snail::Application* Snail::CreateApplication()
{
	return new Example();
}