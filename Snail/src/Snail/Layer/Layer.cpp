#include "SNLpch.h"

#include "Layer.h"

namespace Snail {

	Layer::Layer(const std::string& debugName)
		: m_DebugName(debugName) {}

	void Layer::OnAttach() {}
	void Layer::OnDetach() {}
	void Layer::OnUpdate() {}
	void Layer::OnEvent(Event& event) {}
}