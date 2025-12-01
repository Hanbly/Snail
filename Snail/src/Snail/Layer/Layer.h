#pragma once

#include "Snail/Core/Core.h"

namespace Snail {

	class SNAIL_API Layer {
	protected:
		std::string m_DebugName;
		bool m_Enabled;
	public:
		Layer(const std::string& debugName = "Layer",
			  const bool& layerEnabled = true);
		virtual ~Layer() = default;
		
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate(const Timestep& ts);
		virtual void OnEvent(Event& event);
		virtual void OnRender() = 0;

		inline const std::string& GetLayerName() const { return m_DebugName; }
		inline const bool& IsLayerEnabled() const { return m_Enabled; }
	};

}


