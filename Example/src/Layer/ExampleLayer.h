#pragma once

#include "Snail\Layer\Layer.h"

class ExampleLayer : public Snail::Layer
{
public:
	ExampleLayer(const std::string& layerName, const bool& layerEnabled)
		: Layer(layerName, layerEnabled) {}

	virtual void OnAttach() override {

	}
	virtual void OnDetach() override {

	}

	inline void OnUpdate() override {
		//SNL_TRACE("ExampleLayer 调用: OnUpdate()");
	}

	inline void OnEvent(Snail::Event& e) {
		//SNL_TRACE("ExampleLayer 调用: OnEvent() {0}", e.ToString());
	}

	inline void OnRender() override {
		//SNL_TRACE("ExampleLayer 调用: OnRender()");
	}
};

