#include "SNLpch.h"

#include "Layer.h"

namespace Snail {

	Layer::Layer(const std::string& debugName, const bool& layerEnabled)
		: m_DebugName(debugName), m_Enabled(layerEnabled) {}

	void Layer::OnAttach() {}
	void Layer::OnDetach() {}
	void Layer::OnUpdate() {}
	void Layer::OnEvent(Event& event) {}
}