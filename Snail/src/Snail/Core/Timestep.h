#pragma once

namespace Snail {

	class Timestep {
	private:
		float m_Length;
	public:
		Timestep(float stepLength)
			: m_Length(stepLength)
		{
		}

		inline const float& GetSeconds() const {
			return m_Length;
		}
	};

}