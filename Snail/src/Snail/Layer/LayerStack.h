#pragma once

#include "Snail/Core.h"

#include "Layer.h"

namespace Snail {

	class SNAIL_API LayerStack {
	private:
		std::vector<Layer*> m_Layers;
		// 普通层的插入指针
		std::vector<Layer*>::iterator m_InsertPointer;
	public:
		LayerStack();
		~LayerStack();

		// push/pop normal layer
		void PushNorLayer(Layer* norLayer);
		void PopNorLayer(Layer* norLayer);
		// push/pop Top layer
		void PushOverLayer(Layer* overLayer);
		void PopOverLayer(Layer* overLayer);
		
		std::vector<Layer*>::iterator GetBegin();
		std::vector<Layer*>::iterator GetEnd();
		std::vector<Layer*>::const_iterator GetBegin() const;
		std::vector<Layer*>::const_iterator GetEnd() const;
	};

}


