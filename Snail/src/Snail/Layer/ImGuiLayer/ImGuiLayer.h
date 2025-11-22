#pragma once

#include "Snail/Core.h"

#include "Snail/Layer/Layer.h"

namespace Snail {

	class SNAIL_API ImGuiLayer : public Layer {
	private:

	public:
		ImGuiLayer(const std::string& layerName = "ImGuiLayer",
				   const bool& layerEnabled = true);
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(Event& event) override;

	};

}
