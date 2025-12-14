#pragma once

#include "Snail.h"

namespace Snail {

	class Panel {
	private:
		std::string m_Tag;
		uint32_t m_Width;
		uint32_t m_Height;
	public:
		virtual ~Panel() = default;

		
	};

}