#include "SNLpch.h"

#include "LayerStack.h"

namespace Snail {

	LayerStack::LayerStack() {
		m_InsertPointer = m_Layers.begin();
	}

	LayerStack::~LayerStack() {
		for (Layer* layer : m_Layers) {
			delete layer;
		}
		m_Layers.clear();
	}

	void LayerStack::PushNorLayer(Layer* norLayer)
	{
		if (norLayer == nullptr) {
			SNL_CORE_WARN("LayerStack warning: 推入普通层失败: 试图使用空指针!");
			return;
		}
		m_InsertPointer = m_Layers.insert(m_InsertPointer, norLayer) + 1;
	}

	void LayerStack::PopNorLayer(Layer* norLayer)
	{
		auto it = std::find(m_Layers.begin(), m_InsertPointer, norLayer);
		// it迭代器在普通层的范围内
		if (it < m_InsertPointer) {
			m_Layers.erase(it);
			m_InsertPointer--;
		}
		else {
			SNL_CORE_WARN("LayerStack warning: 弹出普通层失败: 超出层栈边界!");
		}
	}

	void LayerStack::PushOverLayer(Layer* overLayer)
	{
		if (overLayer == nullptr) {
			SNL_CORE_WARN("LayerStack warning: 推入覆盖层失败: 试图使用空指针!");
			return;
		}
		m_Layers.emplace_back(overLayer);
	}

	void LayerStack::PopOverLayer(Layer* overLayer)
	{
		auto it = std::find(m_InsertPointer, m_Layers.end(), overLayer);
		// it迭代器在覆盖层的范围内
		if (it < m_Layers.end()) {
			m_Layers.erase(it);
		}
		else {
			SNL_CORE_WARN("LayerStack warning: 弹出覆盖层失败: 超出层栈边界!");
		}
	}

	std::vector<Layer*>::iterator LayerStack::begin() {
		return m_Layers.begin();
	}
	std::vector<Layer*>::iterator LayerStack::end() {
		return m_Layers.end();
	}
	std::vector<Layer*>::reverse_iterator LayerStack::rbegin() { 
		return m_Layers.rbegin(); 
	}
	std::vector<Layer*>::reverse_iterator LayerStack::rend() { 
		return m_Layers.rend(); 
	}
}
