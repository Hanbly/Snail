#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Core/Timestep.h"

namespace Snail {

	class SNAIL_API Layer {
	protected:
		std::string m_LayerName;
		bool m_Enabled;
	public:
		Layer(const std::string& layerName, const bool& layerEnabled);
		virtual ~Layer() = default;
		
		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void OnUpdate(const Timestep& ts);
		virtual void OnEvent(Event& event);
		virtual void OnRender() = 0;

		virtual void OnImGuiRender() {}

		inline const std::string& GetLayerName() const { return m_LayerName; }
		inline const bool& IsLayerEnabled() const { return m_Enabled; }
	};

}


