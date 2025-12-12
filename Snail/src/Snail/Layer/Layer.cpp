#include "SNLpch.h"

#include "Layer.h"

namespace Snail {

	Layer::Layer(const std::string& layerName, const bool& layerEnabled)
		: m_LayerName(layerName), m_Enabled(layerEnabled) {}

	void Layer::OnUpdate(const Timestep& ts) {}
	void Layer::OnEvent(Event& event) {}
}