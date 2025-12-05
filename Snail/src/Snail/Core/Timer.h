#pragma once

#include <chrono>

namespace Snail {

	template<typename Fn>
	class Timer {
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
		const char* m_Name;
		Fn m_Function;
	public:
		Timer(const char* name, Fn&& func)
			: m_Name(name), m_Function(func)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~Timer()
		{
			auto EndTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(EndTimepoint).time_since_epoch().count();
			float duration = (end - start) * 0.001f;

			m_Function({ m_Name, duration });
		}
	};

}
