#pragma once

#include "Snail/Core.h"

namespace Snail {

	class SNAIL_API Layer {
	protected:
		std::string m_DebugName;
		bool m_Enabled;
	public:
		Layer(const std::string& debugName = "Layer",
			  const bool& layerEnabled = true);
		virtual ~Layer() = default;
		
		virtual void OnAttach();
		virtual void OnDetach();
		virtual void OnUpdate();
		virtual void OnEvent(Event& event);

		inline const std::string& GetLayerName() const { return m_DebugName; }
		inline const bool& IsLayerEnabled() const { return m_Enabled; }
	};

}


