#include "SNLpch.h"

#include "Layer.h"

namespace Snail {

	Layer::Layer(const std::string& debugName, const bool& layerEnabled)
		: m_DebugName(debugName), m_Enabled(layerEnabled) {}

	void Layer::OnUpdate(const Timestep& ts) {}
	void Layer::OnEvent(Event& event) {}
}