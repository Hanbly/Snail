#pragma once

#include "Snail/Core/Core.h"
#include "Snail/Core/Macro.h"

#include "Snail/Layer/Layer.h"

#include "Snail/Events/ApplicationEvent.h"
#include "Snail/Events/KeyboardEvent.h"
#include "Snail/Events/MouseEvent.h"

namespace Snail {

	class SNAIL_API ImGuiLayer : public Layer {
	private:
		bool m_BlockMouseEvents = true;
		bool m_BlockKeyEvents = true;
	public:
		ImGuiLayer(const std::string& layerName = "ImGuiLayer",
				   const bool& layerEnabled = true);
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(const Timestep& ts) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnRender() override;

		void BeginImGui();
		void OnImGuiRender();
		void EndImGui();

		inline void BlockMouseEvents(const bool& status) { m_BlockMouseEvents = status; }
		inline void BlockKeyEvents(const bool& status) { m_BlockKeyEvents = status; }

	private:
		/*bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnMouseButtonPress(MousePressEvent& e);
		bool OnMouseButtonRelease(MouseReleaseEvent& e);
		bool OnMouseMove(MouseMoveEvent& e);
		bool OnMouseScroll(MouseScrollEvent& e);
		bool OnKeyboardPress(KeyPressEvent& e);
		bool OnKeyboardRelease(KeyReleaseEvent& e);
		bool OnKeyboardType(KeyTypeEvent& e);*/
	};

}
