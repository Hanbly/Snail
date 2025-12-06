#pragma once

namespace Snail {

	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Time(time)
		{
		}

		// 允许隐式转换为 float，方便直接相乘
		operator float() const { return m_Time; }

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }

		static float GetTime();

	private:
		float m_Time;
	};

}